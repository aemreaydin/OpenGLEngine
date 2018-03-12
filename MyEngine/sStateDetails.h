#ifndef _ANIMATION_STATE_DETAILS_HG_
#define _ANIMATION_STATE_DETAILS_HG_

struct sStateDetails
{
	std::string name;
	float curTime;
	float totalTime;
	float frameStepTime;

	sStateDetails() : curTime(0.0f), totalTime(0.0f), frameStepTime(0.0f) {}

	bool IncrementTime(bool bResetToZero = true)
	{
		bool isReset = false;

		this->curTime += this->frameStepTime;
		if (this->curTime >= this->totalTime)
		{
			this->curTime = 0.0f;
			isReset = true;
		}
		return isReset;
	}
};

#endif