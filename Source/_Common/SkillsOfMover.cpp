#include "StdAfx.h"
#include "SkillsOfMover.h"


const ItemProp * SKILL::GetProp() const {
  return prj.GetSkillProp(dwSkill);
}

