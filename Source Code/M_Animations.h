#ifndef __M_ANIMATIONS_H__
#define __M_ANIMATIONS_H__

#include "Globals.h"
#include <map>

#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/Quat.h"

class R_Animation;
struct Channel;

struct aiAnimation;
struct aiNodeAnim;


namespace Importer
{
	namespace Animations
	{
		void Import(const aiAnimation* animation, R_Animation* rAnimation);
		uint64 Save(const R_Animation* rAnimation, char** buffer);
		void Load(const char* buffer, R_Animation* rAnimation);

		namespace Private
		{
			void ImportChannel(const aiNodeAnim* node, Channel& channel);

			uint CalcChannelSize(const Channel& channel);
			void SaveChannel(const Channel& channel, char** cursor);
			void SaveChannelKeys(const std::map<double, float3>&, char** cursor);
			void SaveChannelKeys(const std::map<double, Quat>&, char** cursor);

			void LoadChannel(Channel& channel, const char** cursor);
			void LoadChannelKeys(std::map<double, float3>& map, const char** cursor, uint size);
			void LoadChannelKeys(std::map<double, Quat>& map, const char** cursor, uint size);
		}
	}
}
#endif