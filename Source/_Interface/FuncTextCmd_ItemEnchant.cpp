#include "StdAfx.h"
#include "User.h"


/*
 INTERFACE:
namespace SqKItemEnchant {
  void ItemEnchant(CScanner & scanner, CUser * user, CItemElem * item);
}

*/

/**
 * Code for the /itemenchant command.
 *
 * Enables to modify an item enchantement by typing
 * "/itemenchant -level 10 -element fire 7"
 *
 * Dashes indicates subcommands, and every following non dashed parameter
 * is an argument for the subcommand.
 *
 * Original author: SquonK, 2021-03
 * Link: https://gist.github.com/SPSquonK/595c0dfa7d97f180a381c480c57f59f1
 * License: http://squonk.fr/SquonK-Hidden-Boss-License.txt
 */
namespace SqKItemEnchant {
  /** Subcommand Handler Manager */
  class Enchanter {
    struct Parameters {
      const std::vector<CString> & args;
      CUser & user;
      CItemElem & item;
    };

    struct Handler {
      using Function = bool(Parameters);

      const char * usage;
      Function * functionHandler;
    };

    std::map<CString, Handler> handlers;

  public:
    // Available options
    Enchanter() {
      // /!\ Command name must be lower case
      handlers["level"] = Handler{ "[level]", &LevelHandler };
      handlers["element"] = Handler{ "[elementname|none] [level]?", ElementHandler };

      handlers["piercing"] = Handler{ "[quantity] [Item Name]...",
          PiercingHandler<UI::Piercing::Kind::Regular, MAX_PIERCING_WEAPON>
      };
      handlers["jewel"] = Handler{ "[quantity] [Item Name]...",
          PiercingHandler<UI::Piercing::Kind::Ultimate, MAX_PIERCING_ULTIMATE>
      };
    }

    static bool LevelHandler(Parameters p) {
      if (p.args.size() != 1) return false;
      const char * asString = p.args[0].GetString();
      const int targetLevel = std::atoi(asString);
      p.user.UpdateItem(p.item, UI::AbilityOption::Set(targetLevel));
      return true;
    }

    static bool ElementHandler(Parameters p) {
      if (p.args.size() == 0) return false;
      if (p.args.size() > 2) return false;

      CString elementName = p.args[0];
      elementName.MakeLower();

      SAI79::ePropType element;

      if (elementName == "fire") {
        element = SAI79::ePropType::FIRE;
      } else if (elementName == "water") {
        element = SAI79::ePropType::WATER;
      } else if (elementName == "electric" || elementName == "elec" || elementName == "electricity") {
        element = SAI79::ePropType::ELECTRICITY;
      } else if (elementName == "ground" || elementName == "earth") {
        element = SAI79::ePropType::EARTH;
      } else if (elementName == "wind" || elementName == "air") {
        element = SAI79::ePropType::WIND;
      } else if (elementName == "neutral" || elementName == "none" || elementName == "clean") {
        if (p.args.size() != 1) return false;
        element = SAI79::ePropType::NO_PROP;
      } else {
        return false;
      }

      if (element == SAI79::ePropType::NO_PROP) {
        p.user.UpdateItem(p.item, UI::Element::None());
      } else if (p.args.size() == 1) {
        p.user.UpdateItem(p.item, UI::Element::Change(element));
      } else if (p.args.size() == 2) {
        const int level = std::stoi(p.args[1].GetString());
        p.user.UpdateItem(p.item, UI::Element{ .kind = static_cast<BYTE>(element), .abilityOption = level });
      }

      return true;
    }

    template<UI::Piercing::Kind kind, size_t MAX_SLOTS>
    static bool PiercingHandler(Parameters p) {
      // Transform parameters to the list of items to socket
      std::vector<DWORD> newPiercings;

      size_t nextQuantity = 1;

      for (const auto & word : p.args) {
        if (word[0] >= '0' && word[0] <= '9') {
          nextQuantity = std::atol(word.GetString());

          if (nextQuantity > MAX_SLOTS || nextQuantity == 0) {
            return false;
          }
        } else {
          const ItemProp * itemProp = prj.GetItemProp(word.GetString());
          if (!itemProp) {
            CString unknownObject = "Unknown Object in Piercing request :";
            unknownObject += word;
            p.user.AddText(unknownObject);
            return false;
          }

          for (int i = 0; i != nextQuantity; ++i) {
            newPiercings.push_back(itemProp->dwID);
          }
        }
      }

      // Apply the new piercings
      if (newPiercings.size() > MAX_SLOTS) {
        return false;
      }

      p.user.UpdateItem(p.item, UI::Piercing::Size{
        .kind = kind,
        .newSize = static_cast<int>(newPiercings.size())
        });

      for (size_t i = 0; i != newPiercings.size(); ++i) {
        p.user.UpdateItem(p.item,
          UI::Piercing::Item{
            .kind = kind,
            .position = static_cast<int>(i),
            .itemId = newPiercings[i]
          });
      }

      return true;
    }

    // Backend
    [[nodiscard]] CString GetValidCommands() const {
      CString validCommands = "";

      bool isFirst = true;
      for (const auto & handlerKey : handlers | std::views::keys) {
        if (!isFirst) validCommands += ", ";
        else          isFirst = false;

        validCommands += handlerKey;
      }

      return validCommands;
    }

    void OnRequest(CUser * user, CItemElem * target, const std::map<CString, std::vector<CString>> & request) const {
      for (const auto & [key, parameters] : request) {
        const auto it = handlers.find(key);

        if (it == handlers.end()) {
          CString unknownCommand;
          unknownCommand.Format("Unknown command %s - Valid commands are:", key.GetString());
          unknownCommand += GetValidCommands();
          user->AddText(unknownCommand.GetString() /* , 0x00FF0000 */);
        } else {
          const bool success = it->second.functionHandler(Parameters{ parameters, *user, *target });
          if (!success) {
            CString error;
            error.Format("Invalid use of %s - Usage: %s", key.GetString(), it->second.usage);
            user->AddText(error.GetString() /*, 0x00FF0000 */ );
          }
        }
      }
    }
  };

  /* Static Global variable to keep somewhere the map */
  static const Enchanter s_SqKItemEnchanter;

  /** Item Enchant command management entry point */
  void ItemEnchant(CScanner & scanner, CUser * user, CItemElem * item) {
    std::map<CString, std::vector<CString>> parameters;
    auto currentParameter = parameters.end();

    const char * helpPrefix = "";

    while (true) {
      scanner.GetToken();
      if (scanner.tok == FINISHED) break;

      if (scanner.Token == '-') {
        // New sub command
        scanner.GetToken();
        CString commandName = scanner.Token;
        commandName.MakeLower();
        auto [it, success] = parameters.emplace(commandName, std::vector<CString>{});

        if (!success) {
          helpPrefix = "Twice the same command - ";
          break;
        }

        currentParameter = it;
      } else if (currentParameter == parameters.end()) {
        // Invalid use of the command: show help
        helpPrefix = "Invalid use - ";
        break;
      } else {
        // New arg
        currentParameter->second.emplace_back(scanner.Token.GetString());
      }
    }

    if (currentParameter == parameters.end()) {
      CString validCommands = helpPrefix;
      validCommands += "Known Commands: "
        + s_SqKItemEnchanter.GetValidCommands()
        + " - Use: (-commandName [param1] [param2] ...) ...";

      user->AddText(validCommands.GetString());
    } else {
      s_SqKItemEnchanter.OnRequest(user, item, parameters);
    }
  }
}
