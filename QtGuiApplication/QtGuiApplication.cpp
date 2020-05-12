#include "QtGuiApplication.h"
#include "Agora/LiveAgora.h"

#define C_MAIN_WINDOW_W		1280
#define C_MAIN_WINDOW_H		720

#define C_TITLE_GEOMETRY_X	0
#define C_TITLE_GEOMETRY_Y	0
#define C_TITLE_GEOMETRY_W	C_MAIN_WINDOW_W
#define C_TITLE_GEOMETRY_H	20

#define C_SEAT_GEOMETRY_X	0
#define C_SEAT_GEOMETRY_Y	C_TITLE_GEOMETRY_H
#define C_SEAT_GEOMETRY_W	C_MAIN_WINDOW_W
#define C_SEAT_GEOMETRY_H	110

#define C_BOARD_GEOMETRY_X	0
#define C_BOARD_GEOMETRY_Y	(C_SEAT_GEOMETRY_Y + C_SEAT_GEOMETRY_H)
#define C_BOARD_GEOMETRY_W	C_MAIN_WINDOW_W
#define C_BOARD_GEOMETRY_H	590

#define C_TITLE_BGC			"#FFFF00"
#define C_SEAT_BGC			"#FFFFFF"
#define C_BOARD_BGC			"#C5C5C5"
#define C_COLOR_BLACK		"#000000"

#define C_SEAT_M_W			188
#define C_SEAT_M_H			106
#define C_SEAT_M_B			2

#define C_BOARD_M_W			(C_SEAT_M_W * 3)
#define C_BOARD_M_H			(C_SEAT_M_H * 3)
#define C_BOARD_M_B			C_SEAT_M_B


#define	C_TEST_ID			"4240c2f3c0e5424bb7894342b6ca6e1e"
#define	C_TEST_CHANNEL		"XiaoStore"

#if 0

CSeatListModel::CSeatListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int CSeatListModel::rowCount(const QModelIndex &parent) const
{
	int iRet = parent.isValid() ? 0 : m_pixmaps.size();
	return iRet;
}

QVariant CSeatListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DecorationRole)
	{
		int iRow = index.row();
		return QIcon(m_pixmaps.value(iRow));
	}
	else
	{
		return QVariant();
	}
}

Qt::ItemFlags CSeatListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
		return (QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled);

	return Qt::ItemIsDropEnabled;
}

void CSeatListModel::InsertPixmap(const QPixmap &pixmap)
{
	int row = m_pixmaps.size();
	beginInsertRows(QModelIndex(), row, row);
	m_pixmaps.insert(row, pixmap);
	endInsertRows();
}


CSeatListWidget::CSeatListWidget(QWidget *parent)
	: QListWidget(parent)
{
}

void CSeatListWidget::addPiece(const QString &strFile)
{
	QListWidgetItem *pieceItem = new QListWidgetItem(this);
	pieceItem->setIcon(QIcon(QPixmap(strFile)));
	pieceItem->setData(Qt::UserRole, QVariant(strFile));
}

void CSeatListWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
	QListWidgetItem *item = currentItem();

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	QString strFile = qvariant_cast<QString>(item->data(Qt::UserRole));
	dataStream << strFile;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData(CSeatListWidget::puzzleMimeType(), itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
		delete takeItem(row(item));
}
#endif


CBoardWidget::CBoardWidget(QWidget *parent)
	: QWidget(parent)
{
	setAcceptDrops(true);
}


void CBoardWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat(CSeatWidget::MimeType())
		|| event->mimeData()->hasFormat(CBoardWidget::MimeType())
		)
	{
		QtGuiApplication::AcceptDropEvent(event, this);
	}
	else
	{
		event->ignore();
	}
}

QWidget *CBoardWidget::NewWidget(const QString &strFile)
{
	QWidget *pWidget = new QWidget(this);
	pWidget->resize(C_BOARD_M_W, C_BOARD_M_H);
	pWidget->setStyleSheet(QString("border-image:url(") + strFile + ");");
	pWidget->show();
	pWidget->setAttribute(Qt::WA_DeleteOnClose);

	m_listWidgets.push_back(pWidget);
	m_mapWidgetStrings[pWidget] = strFile;
	m_mapStringWidgets[strFile] = pWidget;

	return pWidget;
}

void CBoardWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat(CSeatWidget::MimeType())) {
		QByteArray pieceData = event->mimeData()->data(CSeatWidget::MimeType());
		QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		QString strFile;
		dataStream >> strFile;

		NewWidget(strFile)->move(event->pos() - (QPoint(C_BOARD_M_W, C_BOARD_M_H) / 2));

		QtGuiApplication::AcceptDropEvent(event, this);
	}
	else if (event->mimeData()->hasFormat(CBoardWidget::MimeType())) {
		QByteArray pieceData = event->mimeData()->data(CBoardWidget::MimeType());
		QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		QString strFile;
		dataStream >> strFile;

		QtGuiApplication::AcceptDropEvent(event, this);

		m_mapStringWidgets[strFile]->move(event->pos() - (QPoint(C_BOARD_M_W, C_BOARD_M_H) / 2));
	}
	else {
		event->ignore();
	}
}

void CBoardWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget *child = static_cast<QWidget*>(childAt(event->pos()));
	if (!child)
		return;

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	QString strFile = m_mapWidgetStrings[child];
	dataStream << strFile;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData(CBoardWidget::MimeType(), itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	Qt::DropAction qtDropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
	if (qtDropAction == Qt::CopyAction) {
		child->hide();
	}
}

void CBoardWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	QWidget *child = static_cast<QWidget*>(childAt(event->pos()));
	if (!child)
		return;

	child->hide();

	QString strFile = m_mapWidgetStrings[child];
	m_pSeatWidget->ShowWidget(strFile);
}


CBoardMdiSubWindow::CBoardMdiSubWindow(QWidget *parent)
	: QMdiSubWindow(parent)
{
}

void CBoardMdiSubWindow::mousePressEvent(QMouseEvent *event)
{
}

void CBoardMdiSubWindow::moveEvent(QMoveEvent *event)
{
	QPoint pos = event->pos();
	QPoint oldPos = event->oldPos();
	qDebug("[%s] pos(%d, %d), oldPos(%d, %d)", __FUNCTION__, pos.rx(), pos.ry(), oldPos.rx(), oldPos.ry());
	QWidget::moveEvent(event);
}


CSeatWidget::CSeatWidget(QWidget *parent)
	: QLabel(parent)
{
	setAcceptDrops(true);
}

void CSeatWidget::addPiece(const QString &strFile)
{
	QLabel *pLabel = new QLabel(this);
	pLabel->setPixmap(QPixmap(strFile));
	pLabel->resize(C_SEAT_M_W, C_SEAT_M_H);
	pLabel->setScaledContents(true);

	int iCount = m_mapWidgetStrings.count();
	pLabel->move(iCount * (C_SEAT_M_W + C_SEAT_M_B) + C_SEAT_M_B, C_SEAT_M_B);

	m_listWidgets.push_back(pLabel);
	m_mapWidgetStrings[pLabel] = strFile;
	m_mapStringWidgets[strFile] = pLabel;
}

void *CSeatWidget::PieceWinId(const QString &strFile)
{
	auto it = m_mapStringWidgets.find(strFile);
	if (it == m_mapStringWidgets.end())
	{
		return NULL;
	}

	return (void *)it.value()->winId();
}

void CSeatWidget::mousePressEvent(QMouseEvent *event)
{
	QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
	if (!child)
		return;

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << m_mapWidgetStrings[child];

	QMimeData *mimeData = new QMimeData;
	mimeData->setData(CSeatWidget::MimeType(), itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	Qt::DropAction qtDropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
	if (qtDropAction == Qt::CopyAction) {
		child->hide();
	}
}

void CSeatWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat(CSeatWidget::MimeType())
		|| event->mimeData()->hasFormat(CBoardWidget::MimeType())
		)
	{
		QtGuiApplication::AcceptDropEvent(event, this);
	}
	else
	{
		event->ignore();
	}
}

void CSeatWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat(CSeatWidget::MimeType())) {
		//QByteArray pieceData = event->mimeData()->data(CSeatWidget::MimeType());
		//QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		//QString strFile;
		//dataStream >> strFile;

		QtGuiApplication::AcceptDropEvent(event, this);
	}
	else if (event->mimeData()->hasFormat(CBoardWidget::MimeType())) {
		QByteArray pieceData = event->mimeData()->data(CBoardWidget::MimeType());
		QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		QString strFile;
		dataStream >> strFile;

		m_mapStringWidgets[strFile]->show();

		QtGuiApplication::AcceptDropEvent(event, this);
	}
	else {
		event->ignore();
	}
}

void CSeatWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	QWidget *child = static_cast<QWidget*>(childAt(event->pos()));
	if (!child)
		return;

	child->hide();

	QString strFile = m_mapWidgetStrings[child];
	m_pBoardWidget->ShowWidget(strFile);
}

QtGuiApplication::QtGuiApplication(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);

	resize(C_MAIN_WINDOW_W, C_MAIN_WINDOW_H);
	//setFixedWidth(C_MAIN_WINDOW_W);
	//setFixedHeight(C_MAIN_WINDOW_H);

	m_pTitle = new QWidget(this);
	m_pTitle->setGeometry(C_TITLE_GEOMETRY_X, C_TITLE_GEOMETRY_Y, C_TITLE_GEOMETRY_W, C_TITLE_GEOMETRY_H);
	m_pTitle->setStyleSheet(QString("background-color:") + C_TITLE_BGC + ";");

	m_pSeat = new CSeatWidget(this);
	m_pSeat->setGeometry(C_SEAT_GEOMETRY_X, C_SEAT_GEOMETRY_Y, C_SEAT_GEOMETRY_W, C_SEAT_GEOMETRY_H);
	m_pSeat->setStyleSheet(QString("background-color:") + C_SEAT_BGC + ";");

	m_pBoard = new CBoardWidget(this);
	m_pBoard->setGeometry(C_BOARD_GEOMETRY_X, C_BOARD_GEOMETRY_Y, C_BOARD_GEOMETRY_W, C_BOARD_GEOMETRY_H);
	m_pBoard->setStyleSheet(QString("background-color:") + C_BOARD_BGC + ";");


#if 0
	m_pListView = new QListView(m_pSeat);
	m_pListView->setGeometry(0, 0, C_SEAT_GEOMETRY_W, C_SEAT_GEOMETRY_H);
	m_pListView->setViewMode(QListView::IconMode);	//list row first
	m_pListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/* There are some differences in height, but the impact is not significant */
	m_pListView->setIconSize(QSize(C_SEAT_M_W, C_SEAT_M_H));

	/* prevent move in col */
	m_pListView->setGridSize(QSize(C_SEAT_M_W + C_SEAT_M_B, C_SEAT_M_H));
	m_pListView->setMovement(QListView::Snap);

	m_pSeatListModel = new CSeatListModel(m_pSeat);
	m_pSeatListModel->InsertPixmap(QPixmap("Resources/d1.png"));
	m_pSeatListModel->InsertPixmap(QPixmap("Resources/d2.png"));
	m_pSeatListModel->InsertPixmap(QPixmap("Resources/d3.png"));
	m_pListView->setModel(m_pSeatListModel);
#endif

#if 0
	m_pListWidget = new QListWidget(m_pSeat);
	m_pListWidget->setGeometry(0, 0, C_SEAT_GEOMETRY_W, C_SEAT_GEOMETRY_H);
	m_pListWidget->setViewMode(QListView::IconMode);	//list row first
	m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/* There are some differences in height, but the impact is not significant */
	m_pListWidget->setIconSize(QSize(C_SEAT_M_W, C_SEAT_M_H));

	/* prevent move in col */
	m_pListWidget->setGridSize(QSize(C_SEAT_M_W + C_SEAT_M_B, C_SEAT_M_H));
	m_pListWidget->setMovement(QListView::Snap);

	QListWidgetItem *pListWidgetItem = new QListWidgetItem(m_pListWidget);
	pListWidgetItem->setIcon(QIcon(QPixmap("Resources/d1.png")));
	pListWidgetItem = new QListWidgetItem(m_pListWidget);
	pListWidgetItem->setIcon(QIcon(QPixmap("Resources/d2.png")));
	pListWidgetItem = new QListWidgetItem(m_pListWidget);
	pListWidgetItem->setIcon(QIcon(QPixmap("Resources/d3.png")));
#endif

#if 0
	m_pSeatListWidget = new CSeatListWidget(m_pSeat);
	m_pSeatListWidget->setGeometry(0, 0, C_SEAT_GEOMETRY_W, C_SEAT_GEOMETRY_H);
	m_pSeatListWidget->setViewMode(QListView::IconMode);	//list row first
	m_pSeatListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pSeatListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/* There are some differences in height, but the impact is not significant */
	m_pSeatListWidget->setIconSize(QSize(C_SEAT_M_W, C_SEAT_M_H));

	/* prevent move in col */
	m_pSeatListWidget->setGridSize(QSize(C_SEAT_M_W + C_SEAT_M_B, C_SEAT_M_H));
	m_pSeatListWidget->setMovement(QListView::Snap);
	m_pSeatListWidget->addPiece("Resources/d1.png");
	m_pSeatListWidget->addPiece("Resources/d2.png");
	m_pSeatListWidget->addPiece("Resources/d3.png");
#endif

	m_pBoardWindow = new QMainWindow(this);
	m_pBoardWindow->setGeometry(C_BOARD_GEOMETRY_X, C_BOARD_GEOMETRY_Y, C_BOARD_GEOMETRY_W, C_BOARD_GEOMETRY_H);
	m_pBoardWindow->setStyleSheet(QString("background-color:") + C_BOARD_BGC + ";");
	m_pBoardWindow->setCentralWidget(m_pBoard);

	m_pDockWidget = new QDockWidget("DockWidget", m_pBoardWindow);
	m_pDockWidget->setMinimumSize(200, 200);
	m_pBoardWindow->addDockWidget(Qt::LeftDockWidgetArea, m_pDockWidget);

	m_pWidgetWidget = new QWidget();
	m_pWidgetWidget->setMinimumSize(200, 200);
	m_pDockWidgetWidget = new QDockWidget("DockWidgetWidget", m_pBoardWindow);
	m_pDockWidgetWidget->setWidget(m_pWidgetWidget);
	m_pBoardWindow->addDockWidget(Qt::RightDockWidgetArea, m_pDockWidgetWidget);

	m_pWebEngineView = new QWebEngineView();
	m_pWebEngineView->setMinimumSize(500, 300);
	m_pWebEngineView->setUrl(QUrl("https://www.baidu.com"));
	m_pDockWidgetWebEngineView = new QDockWidget("DockWidgetWebEngineView", m_pBoardWindow);
	m_pDockWidgetWebEngineView->setWidget(m_pWebEngineView);
	m_pBoardWindow->addDockWidget(Qt::BottomDockWidgetArea, m_pDockWidgetWebEngineView);
	m_pBoardWindow->setWindowFlags(Qt::Widget);

	m_pSeat->addPiece("Resources/d1.png");
	m_pSeat->addPiece("Resources/d2.png");
	m_pSeat->addPiece("Resources/d3.png");

	m_pBoard->SetSeatWidget(m_pSeat);
	m_pSeat->SetBoardWidget(m_pBoard);

	m_pQMdiArea = new QMdiArea(m_pBoard);
	m_pQMdiArea->setGeometry(0, 0, C_BOARD_GEOMETRY_W, C_BOARD_GEOMETRY_H);

	m_pQMdiSubWindow = new QMdiSubWindow();
	m_pQMdiSubWindow->resize(300, 200);
	m_pQMdiArea->addSubWindow(m_pQMdiSubWindow);

	m_QWebEngineView = new QWebEngineView();
	m_QWebEngineView->setUrl(QUrl("https://www.baidu.com"));
	m_pQMdiArea->addSubWindow(m_QWebEngineView);

	m_pLive = new CLiveAgora();
	m_pLive->Init(C_TEST_ID);
	m_pLive->Active(m_pSeat->PieceWinId("Resources/d1.png"), C_TEST_CHANNEL);
}
