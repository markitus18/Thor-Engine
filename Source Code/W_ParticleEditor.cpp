#include "W_ParticleEditor.h"

#include "Engine.h"
#include "M_Editor.h"
#include "W_Explorer.h"
#include "Resource.h"
#include "R_ParticleSystem.h"
#include "ParticleModule.h"

#include "ImGui/imgui.h"

W_ParticleEditor::W_ParticleEditor(M_Editor* editor) : DWindow(editor, "Particle Editor")
{

}


void W_ParticleEditor::Draw()
{
	if (!active) return;
	if (!ImGui::Begin("Particle Editor")) { ImGui::End(); return; }
	
	R_ParticleSystem* particleSystem = nullptr;
	if (editor->selectedResources.size() > 0)
	{
		if (editor->selectedResources[0]->GetType() == Resource::Type::PARTICLESYSTEM)
		{
			particleSystem = (R_ParticleSystem*)editor->selectedResources[0];
		}
	}
	if (particleSystem == nullptr)
	{
		ImGui::Text("Select a Particle System to edit it");
	}
	
	if (ImGui::CollapsingHeader("Emitters"))
	{
		if (particleSystem->emitters.size() > 0)
		{
			ImGui::Columns(particleSystem->emitters.size());

			for (uint i = 0; i < particleSystem->emitters.size(); ++i)
			{
				Emitter* emitter = &particleSystem->emitters[i];
				ImGui::PushID(emitter);
				if (ImGui::Selectable(particleSystem->emitters[i].name.c_str(), selectedEmitter == emitter))
					selectedEmitter = emitter;
				ImGui::PopID();
				ImGui::NextColumn();
			}
			ImGui::Separator();
			bool moduleDrawn = true;
			for (uint m = 0; moduleDrawn == true; ++m)
			{
				moduleDrawn = false;
				for (uint e = 0; e < particleSystem->emitters.size(); ++e)
				{
					Emitter* emitter = &particleSystem->emitters[e];
					if (emitter->modules.size() > m)
					{
						moduleDrawn = true;
						ParticleModule* module = emitter->modules[m];
						ImGui::PushID(emitter);
						if (ImGui::Selectable(GetModuleName(module).c_str(), selectedModule == module))
						{
							selectedEmitter = emitter;
							selectedModule = module;
						}
						ImGui::PopID();
					}
					ImGui::NextColumn();
				}
			}
			ImGui::Columns(1);
			ImGui::Separator();
		}
		if (ImGui::Button("Add Emitter"))
		{
			particleSystem->AddDefaultEmitter();
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("Selected Module: ");
		if (selectedModule)
			DrawModuleData(selectedModule);
	}
	
	ImGui::End();
}

std::string W_ParticleEditor::GetModuleName(const ParticleModule* module) const
{
	switch (module->type)
	{
		case(ParticleModule::Type::EmitterBase):
			return "Base";
		case(ParticleModule::Type::EmitterSpawn):
			return "Spawn";
		case(ParticleModule::Type::EmitterArea):
			return "Area";
		case(ParticleModule::Type::ParticlePosition):
			return "Position";
		case(ParticleModule::Type::ParticleRotation):
			return "Rotation";
		case(ParticleModule::Type::ParticleSize):
			return "Size";
		case(ParticleModule::Type::ParticleColor):
			return "Color";
		case(ParticleModule::Type::ParticleLifetime):
			return "Lifetime";
		case(ParticleModule::Type::ParticleVelocity):
			return "Velocity";
	}
}

void W_ParticleEditor::DrawModuleData(ParticleModule* module)
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

void W_ParticleEditor::DrawModule(EmitterBase* module)
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

void W_ParticleEditor::DrawModule(EmitterSpawn* module)
{
	if (ImGui::InputFloat("Spawn Ratio", &module->spawnRatio))
		module->spawnRatio = math::Clamp<float>(module->spawnRatio, 0.0f, module->spawnRatio);
}

void W_ParticleEditor::DrawModule(EmitterArea* module)
{
	ImGui::Text("-- Needs Update --");
}

void W_ParticleEditor::DrawModule(ParticlePosition* module)
{
	ImGui::InputFloat3("Position 1", module->initialPosition1.ptr());
	ImGui::InputFloat3("Position 2", module->initialPosition2.ptr());
}

void W_ParticleEditor::DrawModule(ParticleRotation* module)
{
	ImGui::InputFloat("Rotation 1", &module->initialRotation1);
	ImGui::InputFloat("Rotation 2", &module->initialRotation2);
}

void W_ParticleEditor::DrawModule(ParticleSize* module)
{
	if (ImGui::InputFloat("Size 1", &module->initialSize1))
		module->initialSize1 = math::Clamp<float>(module->initialSize1, 0.0f, module->initialSize1);
	if (ImGui::InputFloat("Size 2", &module->initialSize2))
		module->initialSize2 = math::Clamp<float>(module->initialSize2, 0.0f, module->initialSize2);
}

void W_ParticleEditor::DrawModule(ParticleColor* module)
{
	ImGui::ColorEdit4("Color 1", module->initialColor1.ptr());
	ImGui::ColorEdit4("Color 2", module->initialColor2.ptr());
}

void W_ParticleEditor::DrawModule(ParticleLifetime* module)
{
	if (ImGui::InputFloat("Lifetime 1", &module->initialLifetime1))
		module->initialLifetime1 = math::Clamp<float>(module->initialLifetime1, 0.0f, module->initialLifetime1);
	if (ImGui::InputFloat("Lifetime 2", &module->initialLifetime2))
		module->initialLifetime2 = math::Clamp<float>(module->initialLifetime2, 0.0f, module->initialLifetime2);
}

void W_ParticleEditor::DrawModule(ParticleVelocity* module)
{
	ImGui::InputFloat4("Velocity 1", module->initialVelocity1.ptr());
	ImGui::InputFloat4("Velocity 2", module->initialVelocity2.ptr());
}