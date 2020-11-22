#ifndef __IMGUI_HELPER_H__
#define __IMGUI_HELPER_H__

namespace ImGuiHelper
{
	void PushInvisibleButtonStyle();
	void PopInvisibleButtonStyle();

	template <class T>
	bool FlagSelection(unsigned __int64& currentFlag)
	{
		T::Flags it = 1 << 0;
		for (T::Flags i = 0; it < T::Max; ++i, it = 1 << i)
		{
			if (ImGui::Selectable(T::str[i + 1].c_str(), currentFlag & it))
			{
				currentFlag = it;
				return true;
			}
		}
		return false;
	}

	template <class T>
	bool FlagMultiSelection(unsigned __int64& currentFlag)
	{
		bool ret = false;

		T::Flags it = 1 << 0;
		for (T::Flags i = 0; it < T::Max; ++i, it = 1 << i)
		{
			bool selected = currentFlag & it;
			selected ? ImGui::Bullet() : ImGui::Indent(ImGui::GetStyle().FramePadding.x * 5.5f);

			if (currentFlag & it)
			{
				//ImGui::Bullet();
			}
			else
			{
				
			}

			if (ImGui::Selectable(T::str[i + 1].c_str(), false))
			{
				currentFlag ^= it;
				ret = true;
			}
			if (!selected)
				ImGui::Unindent(ImGui::GetStyle().FramePadding.x * 5.5f);
		}

		return ret;
	}
}



#endif //!__IMGUI_HELPER_H__
