#ifndef _ANIMATION_STATE_HG_
#define _ANIMATION_STATE_HG_

#include <string>
#include <vector>
#include <stack>
#include "sStateDetails.h"

struct sAnimationState
{
	std::vector<sStateDetails> vecAnimationsToPlay;
	std::stack<sStateDetails> stackAnimationsToPlay;
	sStateDetails defaultAnimation;
};

#endif // !_ANIMATION_STATE_HG_
