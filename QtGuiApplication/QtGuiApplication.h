#pragma once

#include <QtWidgets/QMainWindow>

#include <QAbstractListModel>
#include <QListWidget>
#include <QLabel>
#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QPainter>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QWebEngineView>
#include <QDockWidget>

#include "ui_QtGuiApplication.h"

#if 0
class CSeatListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	CSeatListModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	void InsertPixmap(const QPixmap &pixmap);

private:
	QVector<QPixmap> m_pixmaps;
};


class CSeatListWidget : public QListWidget
{
	Q_OBJECT

public:
	CSeatListWidget(QWidget *parent = nullptr);

	static QString puzzleMimeType() { return QStringLiteral("image/x-puzzle-piece"); }

	void addPiece(const QString &strFile);

protected:
	void startDrag(Qt::DropActions supportedActions) override;
};
#endif


class CBoardWidget;

class CSeatWidget : public QLabel
{
	Q_OBJECT

public:
	CSeatWidget(QWidget *parent = nullptr);

	static QString MimeType() { return QStringLiteral("image/seat-piece"); }

	void addPiece(const QString &strFile);

	inline void ShowWidget(const QString &strFile)
	{
		m_mapStringWidgets[strFile]->show();
	}

	inline void SetBoardWidget(CBoardWidget *pBoardWidget)
	{
		m_pBoardWidget = pBoardWidget;
	}

protected:
	void mousePressEvent(QMouseEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	QList<QWidget *> m_listWidgets;
	QMap<QWidget *, QString> m_mapWidgetStrings;	//use mousePressEvent
	QMap<QString, QWidget *> m_mapStringWidgets;	//use dropEvent This::MimeType

	CBoardWidget *m_pBoardWidget;
};


class CBoardWidget : public QWidget
{
	Q_OBJECT

public:
	CBoardWidget(QWidget *parent = nullptr);

	static QString MimeType() { return QStringLiteral("image/board-piece"); }

	QWidget *NewWidget(const QString &strFile);

	inline void ShowWidget(const QString &strFile)
	{
		QWidget *pWidget = NewWidget(strFile);
		QPoint pCenter((width() - pWidget->width()) / 2, (height() - pWidget->height()) / 2);
		pWidget->move(pCenter);
	}

	inline void SetSeatWidget(CSeatWidget *pSeatWidget)
	{
		m_pSeatWidget = pSeatWidget;
	}

protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;

	void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	QList<QWidget *> m_listWidgets;
	QMap<QWidget *, QString> m_mapWidgetStrings;	//use mousePressEvent
	QMap<QString, QWidget *> m_mapStringWidgets;	//use dropEvent This::MimeType

	CSeatWidget *m_pSeatWidget;
};


class CBoardMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT

public:
	CBoardMdiSubWindow(QWidget *parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void moveEvent(QMoveEvent *event) override;
};

class QtGuiApplication : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication(QWidget *parent = Q_NULLPTR);

	inline static void AcceptDropEvent(QDropEvent *event, QObject *pAccepter)
	{
		if (event->source() == pAccepter) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else {
			event->acceptProposedAction();
		}
	}

private:
	Ui::QtGuiApplicationClass ui;

	QWidget *m_pTitle;
	CSeatWidget *m_pSeat;
	QMainWindow *m_pBoardWindow;
	CBoardWidget *m_pBoard;

#if 0
	CSeatListModel *m_pSeatListModel;
	QListView *m_pListView;

	QListWidget *m_pListWidget;

	CSeatListWidget *m_pSeatListWidget;
#endif

	QMdiArea *m_pQMdiArea;
	QMdiSubWindow *m_pQMdiSubWindow;
	QWebEngineView *m_QWebEngineView;

	QDockWidget *m_pQDockWidget;

	QDockWidget *m_pDockWidget;

	QWidget *m_pWidgetWidget;
	QDockWidget *m_pDockWidgetWidget;

	QWebEngineView *m_pWebEngineView;
	QDockWidget *m_pDockWidgetWebEngineView;
};
