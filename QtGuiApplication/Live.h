#ifndef __LIVE_H__
#define __LIVE_H__

#include <QString>

/**
1: Uniformly scale the video until it fills the visible boundaries (cropped). One dimension of the video may have clipped contents.
 */
#define LIVE_RENDER_MODE_HIDDEN		1

/**
2: Uniformly scale the video until one of its dimension fits the boundary (zoomed to fit). Areas that are not filled due to disparity in the aspect ratio are filled with black.
 */
#define LIVE_RENDER_MODE_FIT		2

class CLive
{
public:
	CLive() {}
	virtual ~CLive() {}

	virtual bool Init(QString strId = "", QString strLogName = "") { return false; }
	virtual bool Uninit() { return false; }

	virtual bool Active(
		void *pWndId = NULL,
		const QString channel = "",
		uint uRenderType = LIVE_RENDER_MODE_FIT,
		uint uId = 0,
		const QString key = ""
	)
	{
		return false;
	}
	virtual bool Deactive() { return false; }
};

#endif // __LIVE_H__
