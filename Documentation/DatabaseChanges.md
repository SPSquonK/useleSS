# Database changes

- tblSkillPoint(serverindex, PlayerId, ~~SkillID~~, ~~SkillLevel~~, ~~SkillPosition~~, **Skill**)
    - The skill list is stringified into a collection instead of having one
    row per skill
    - We prefer this because all skills are always saved or loaded: individual
    skills are never saved or loaded without the others.
    - Deleting lost skills was surprisingly hard 

