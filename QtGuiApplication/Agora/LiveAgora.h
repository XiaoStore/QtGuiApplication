#ifndef __LIVE_AGORA_H__
#define __LIVE_AGORA_H__

#include "Live.h"

#include <QString>
#include <QObject>

class CLiveAgora : public CLive, public QObject
{
public:
	bool Init(QString strId = "", QString strLogName = "");
	bool Uninit();

	bool Active(
		void *pWndId = NULL,
		const QString channel = "",
		uint uRenderType = LIVE_RENDER_MODE_FIT,
		uint uId = 0,
		const QString key = ""
	);
	bool Deactive();

private:
	QString m_strRoomId;
};

#endif // __LIVE_AGORA_H__
