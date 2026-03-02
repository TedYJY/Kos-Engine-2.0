#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "ScoreManagerScript.h"

class EnemyManagerScript;

class AcidGas : public TemplateSC {
public:

    int     blastDamage = 2;
	float   pushbackForce = 30.f;
	float   staggerDuration = 1.f;
	int     manaCost = 30;
	int     scoreValue = 150;

	utility::GUID blastSfxGUID;

	ScoreManagerScript* scoreManager = nullptr;

    float   lifetime = 1.5f;
    float   currLifetime = 0.f;
    bool    isDying = false;

	utility::GUID enemyDeathSfxGUID_1;
	utility::GUID enemyDeathSfxGUID_2;
	utility::GUID enemyDeathSfxGUID_3;
	
	std::vector<utility::GUID> enemyDeathSfxGUIDs;

    void Start() override {

       

    }

    void Update() override {

    }
    REFLECTABLE(AcidGas, blastDamage, pushbackForce, staggerDuration, manaCost, scoreValue, blastSfxGUID)
};
