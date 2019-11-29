#include "W_ParticleEditor.h"

#include "Application.h"
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
		return;
	}

	if (ImGui::CollapsingHeader("Emitters"))
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

		if (ImGui::Button("Add Emitter"))
		{
			particleSystem->AddDefaultEmitter();
		}
	}
}

void W_ParticleEditor::OnResize()
{

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