#include "W_Emitters.h"

#include "WF_ParticleEditor.h"
#include "R_ParticleSystem.h"
#include "ParticleModule.h"

#include "ImGui/imgui.h"

W_Emitters::W_Emitters(WindowFrame* parent, ImGuiWindowClass* windowClass, int ID) : Window(parent, GetName(), windowClass, ID)
{

}


void W_Emitters::Draw()
{
	WF_ParticleEditor* hostWindow = (WF_ParticleEditor*)parentFrame;

	R_ParticleSystem* particleSystem = hostWindow->particleSystem;
	Emitter* selectedEmitter = hostWindow->selectedEmitter;
	ParticleModule* selectedModule = hostWindow->selectedModule;

	ImGui::SetNextWindowClass(windowClass);
	if (!ImGui::Begin(windowStrID.c_str(), &active)) { ImGui::End(); return; }

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
						selectedEmitter = hostWindow->selectedEmitter = emitter;
						selectedModule = hostWindow->selectedModule = module;
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
		particleSystem->needs_save = true;
	}

	ImGui::End();
}

std::string W_Emitters::GetModuleName(const ParticleModule* module) const
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
	return "Unknown";
}