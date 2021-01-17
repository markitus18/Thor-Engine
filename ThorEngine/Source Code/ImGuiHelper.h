#ifndef __IMGUI_HELPER_H__
#define __IMGUI_HELPER_H__

#include <vector>

namespace ImGuiHelper
{
	void PushInvisibleButtonStyle();
	void PopInvisibleButtonStyle();

	template <class T>
	bool FlagSelection(unsigned __int64& currentFlag, const unsigned __int64* mask = nullptr)
	{
		T::Flags it = 1 << 0;
		for (T::Flags i = 0; it < T::Max; ++i, it = 1 << i)
		{
			if (!mask || *mask & it)
			{
				bool selected = currentFlag & it;
				selected ? ImGui::Bullet() : ImGui::Indent(ImGui::GetStyle().FramePadding.x * 5.5f);

				if (ImGui::Selectable(T::str[i + 1].c_str(), false))
				{
					currentFlag = it;
					return true;
				}

				if (!selected)
					ImGui::Unindent(ImGui::GetStyle().FramePadding.x * 5.5f);
			}
		}
		return false;
	}

	template <class T>
	bool FlagMultiSelection(unsigned __int64& currentFlag, const unsigned __int64* mask = nullptr)
	{
		bool modified = false;

		T::Flags it = 1 << 0;
		for (T::Flags i = 0; it < T::Max; ++i, it = 1 << i)
		{
			if (!mask || *mask & it)
			{
				bool selected = currentFlag & it;
				selected ? ImGui::Bullet() : ImGui::Indent(ImGui::GetStyle().FramePadding.x * 5.5f);

				if (ImGui::Selectable(T::str[i + 1].c_str(), false))
				{
					currentFlag ^= it;
					modified = true;
				}

				if (!selected)
					ImGui::Unindent(ImGui::GetStyle().FramePadding.x * 5.5f);
			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Show All", false))
		{
			memset(&currentFlag, -1, sizeof(currentFlag));
			modified = true;
		}
		if (ImGui::Selectable("Hide All", false))
		{
			memset(&currentFlag, 0, sizeof(currentFlag));
			modified = true;
		}
		return modified;
	}

	template <class T>
	bool ValueSelection(unsigned int& currentIndex, const std::vector<T>& values)
	{
		bool modified = false;

		for (unsigned int i = 0; i < values.size(); ++i)
		{
			bool selected = currentIndex == i;
			selected == true ? ImGui::Bullet() : ImGui::Indent(ImGui::GetStyle().FramePadding.x * 4.8f);

			if (ImGui::Selectable(values[i].toStr().append("##").append(std::to_string(i)).c_str(), false))
			{
				currentIndex = i;
				modified = true;
			}

			if (!selected)
				ImGui::Unindent(ImGui::GetStyle().FramePadding.x * 4.8f);
		}

		return modified;
	}
}



#endif //!__IMGUI_HELPER_H__
