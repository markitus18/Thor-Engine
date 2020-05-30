#include "W_ParticleDetails.h"

#include "WF_ParticleEditor.h"
#include "ParticleModule.h"

#include "ImGui/imgui.h"

W_ParticleDetails::W_ParticleDetails(M_Editor* editor, ImGuiWindowClass* windowClass, WF_ParticleEditor* hostWindow) : Window(editor, GetName(), windowClass), hostWindow(hostWindow)
{

}

void W_ParticleDetails::Draw()
{
	ParticleModule* module = hostWindow->selectedModule;

	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(GetName(), &active)) { ImGui::End(); return; }

	if (module != nullptr)
	{
		switch (module->type)
		{
		case(ParticleModule::Type::EmitterBase):
			DrawModule((EmitterBase*)module); break;
		case(ParticleModule::Type::EmitterSpawn):
			DrawModule((EmitterSpawn*)module); break;
		case(ParticleModule::Type::EmitterArea):
			DrawModule((EmitterArea*)module); break;
		case(ParticleModule::Type::ParticlePosition):
			DrawModule((ParticlePosition*)module); break;
		case(ParticleModule::Type::ParticleRotation):
			DrawModule((ParticleRotation*)module); break;
		case(ParticleModule::Type::ParticleSize):
			DrawModule((ParticleSize*)module); break;
		case(ParticleModule::Type::ParticleColor):
			DrawModule((ParticleColor*)module); break;
		case(ParticleModule::Type::ParticleLifetime):
			DrawModule((ParticleLifetime*)module); break;
		case(ParticleModule::Type::ParticleVelocity):
			DrawModule((ParticleVelocity*)module); break;
		}
	}
	else
	{
		//TODO: Draw generic particle system details
	}


	ImGui::End();
}

void W_ParticleDetails::DrawModule(EmitterBase* module)
{
	ImGui::InputFloat3("Origin: ", module->emitterOrigin.ptr());

	static char* alignmentOptions[9] = { "None", "Screen", "Camera", "LockYZ", "LockYX", "LockXY", "LockXZ", "LockZX", "LockZY" };
	int currentOption = (int)module->alignment;

	if (ImGui::BeginCombo("Alignment", alignmentOptions[(int)currentOption]))
	{
		for (uint i = 0; i < 9; ++i)
		{
			if (ImGui::Selectable(alignmentOptions[i], i == currentOption))
				module->alignment = (EmitterBase::Alignment)i;
		}
		ImGui::EndCombo();
	}
}

void W_ParticleDetails::DrawModule(EmitterSpawn* module)
{
	if (ImGui::InputFloat("Spawn Ratio", &module->spawnRatio))
		module->spawnRatio = math::Clamp<float>(module->spawnRatio, 0.0f, module->spawnRatio);
}

void W_ParticleDetails::DrawModule(EmitterArea* module)
{
	ImGui::Text("-- Needs Update --");
}

void W_ParticleDetails::DrawModule(ParticlePosition* module)
{
	ImGui::InputFloat3("Position 1", module->initialPosition1.ptr());
	ImGui::InputFloat3("Position 2", module->initialPosition2.ptr());
}

void W_ParticleDetails::DrawModule(ParticleRotation* module)
{
	ImGui::InputFloat("Rotation 1", &module->initialRotation1);
	ImGui::InputFloat("Rotation 2", &module->initialRotation2);
}

void W_ParticleDetails::DrawModule(ParticleSize* module)
{
	if (ImGui::InputFloat("Size 1", &module->initialSize1))
		module->initialSize1 = math::Clamp<float>(module->initialSize1, 0.0f, module->initialSize1);
	if (ImGui::InputFloat("Size 2", &module->initialSize2))
		module->initialSize2 = math::Clamp<float>(module->initialSize2, 0.0f, module->initialSize2);
}

void W_ParticleDetails::DrawModule(ParticleColor* module)
{
	ImGui::ColorEdit4("Color 1", module->initialColor1.ptr());
	ImGui::ColorEdit4("Color 2", module->initialColor2.ptr());
}

void W_ParticleDetails::DrawModule(ParticleLifetime* module)
{
	if (ImGui::InputFloat("Lifetime 1", &module->initialLifetime1))
		module->initialLifetime1 = math::Clamp<float>(module->initialLifetime1, 0.0f, module->initialLifetime1);
	if (ImGui::InputFloat("Lifetime 2", &module->initialLifetime2))
		module->initialLifetime2 = math::Clamp<float>(module->initialLifetime2, 0.0f, module->initialLifetime2);
}

void W_ParticleDetails::DrawModule(ParticleVelocity* module)
{
	ImGui::InputFloat4("Velocity 1", module->initialVelocity1.ptr());
	ImGui::InputFloat4("Velocity 2", module->initialVelocity2.ptr());
}