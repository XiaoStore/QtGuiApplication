#include "LiveAgora.h"
#include "agoraobject.h"

#include <QCoreApplication>

bool CLiveAgora::Init(QString strId, QString strLogName)
{
	m_strRoomId.clear();

	QString strDir = QCoreApplication::applicationDirPath();
	if (strLogName.length() <= 0)
	{
		strLogName = "AgoraSDK.log";
	}
	strDir.append("/").append(strLogName);
	CAgoraObject::getInstance(this, strId)->setLogPath(strDir);

	CAgoraObject::getInstance()->enableAudio(true);
	CAgoraObject::getInstance()->enableVideo(true);
	CAgoraObject::getInstance()->EnableWebSdkInteroperability(true);
	CAgoraObject::getInstance()->SetChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	CAgoraObject::getInstance()->SetClientRole(CLIENT_ROLE_BROADCASTER);

	return true;
}

bool CLiveAgora::Uninit()
{
	return true;
}

bool CLiveAgora::Active(
	void *pWndId,
	const QString channel,
	uint uRenderType,
	uint uId,
	const QString key
)
{
	CAgoraObject::getInstance()->LocalVideoPreview((HWND)pWndId, TRUE, (RENDER_MODE_TYPE)uRenderType);
	CAgoraObject::getInstance()->joinChannel(key, channel, uId);

	return true;
}

bool CLiveAgora::Deactive()
{
	return true;
}