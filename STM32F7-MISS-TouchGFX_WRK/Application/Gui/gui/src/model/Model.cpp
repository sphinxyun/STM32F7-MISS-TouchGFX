#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "settings/settings_app.h"

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
}

void Model::incBrightness() {
	SETTINGS_IncBrightness();
}

void Model::decBrightness() {
	SETTINGS_DecBrightness();
}
