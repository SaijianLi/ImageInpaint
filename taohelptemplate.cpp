#include "taohelptemplate.h"
#include "ui_taohelptemplate.h"


TaoHelpTemplate::TaoHelpTemplate(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TaoHelpTemplate)
{
    ui->setupUi(this);

    createToolBar();

    ui->txtBrowserHelpContent->setGeometry(QRect(0, 0, this->width(), this->height() - 120));

//    this->setCentralWidget(ui->txtBrowserHelpContent);
}

TaoHelpTemplate::~TaoHelpTemplate()
{
    delete ui;
}


void TaoHelpTemplate::setHiddenCheckBoxDisplay(bool hidden)
{
    ui->chkBoxDisplay->setHidden(hidden);
}

void TaoHelpTemplate::setHelpContent(QString title, QString strFile) //, QString strHelp)
{
    setWindowTitle(title);

//    ui->lblHelpContent->setTextFormat(Qt::RichText);
//    ui->lblHelpContent->setWordWrap(true);
//    ui->lblHelpContent->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
//    ui->lblHelpContent->setOpenExternalLinks(true);
//    ui->lblHelpContent->setText(strHelp);


    ui->txtBrowserHelpContent->setOpenLinks(true);
    ui->txtBrowserHelpContent->setOpenExternalLinks(true);
    ui->txtBrowserHelpContent->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
//    ui->txtBrowserHelpContent->setHtml(strHelp);
    ui->txtBrowserHelpContent->setSource(QUrl(strFile));

    if (TaoSettings::isDisplayToolHelp(title))
    {
        ui->chkBoxDisplay->setChecked(false);
    }
    else
    {
        ui->chkBoxDisplay->setChecked(true);
    }
}


void TaoHelpTemplate::on_btnClose_clicked()
{
    this->hide();
}


void TaoHelpTemplate::setToolName(QString name)
{
    toolName = name;
}


void TaoHelpTemplate::on_btnVisitWeb_clicked()
{
    QDesktopServices::openUrl(QUrl(tr("www.taoshuiyin.com")));
}


void TaoHelpTemplate::on_chkBoxDisplay_stateChanged(int curState)
{
    if (toolName == "")
        return;

    if (curState == Qt::Checked)
    {
//        QMessageBox::about(this, "title", "checked1");
          TaoSettings::saveDisplayToolHelpOption(toolName, true);
    }
    else
    {
//         QMessageBox::about(this, "title", "checked0");
            TaoSettings::saveDisplayToolHelpOption(toolName, false);
    }
}


void TaoHelpTemplate::createToolBar()
{
    mainToolBar = ui->mainToolBar;

    mainToolBar->setFixedHeight(40);

    mainToolBar->setIconSize(QSize(32, 32));

    mainToolBar->setFont(QFont(QGuiApplication::font().family(), 12));

    btnHome = new QToolButton(this);
    QIcon iconHome(QIcon(tr(".//icons//home_32px.ico")));

    btnHome->setIcon(iconHome);
    btnHome->setToolTip(tr("帮助中心"));
    btnHome->setAutoRaise(true);

    mainToolBar->addWidget(btnHome);
    connect(btnHome, SIGNAL(clicked(bool)), this, SLOT(homeClicked()));

    btnBackward = new QToolButton(this);
    QIcon iconBackward(QIcon(tr(".//icons//arrow_left_32px.ico")));

    btnBackward->setIcon(iconBackward);
    btnBackward->setToolTip(tr("上一页"));
    btnBackward->setAutoRaise(true);

    mainToolBar->addWidget(btnBackward);
    connect(btnBackward, SIGNAL(clicked(bool)), this, SLOT(backwardClicked()));

    btnForward = new QToolButton(this);
    QIcon iconForward(QIcon(tr(".//icons//arrow_right_32px.ico")));

    btnForward->setIcon(iconForward);
    btnForward->setToolTip(tr("下一页"));
    btnForward->setAutoRaise(true);

    mainToolBar->addWidget(btnForward);
    connect(btnForward, SIGNAL(clicked(bool)), this, SLOT(forwardClicked()));
}


void TaoHelpTemplate::homeClicked()
{
    ui->txtBrowserHelpContent->setSource(QUrl("./help/index.html"));
}


void TaoHelpTemplate::backwardClicked()
{
    if (ui->txtBrowserHelpContent->isBackwardAvailable())
        ui->txtBrowserHelpContent->backward();
}


void TaoHelpTemplate::forwardClicked()
{
    if (ui->txtBrowserHelpContent->isForwardAvailable())
        ui->txtBrowserHelpContent->forward();
}


void TaoHelpTemplate::resizeEvent(QResizeEvent *event)
{
    ui->txtBrowserHelpContent->setGeometry(QRect(0, 0, this->width(), this->height() - 120));

    ui->btnClose->setGeometry((this->width() - ui->btnClose->width()) /2, this->height() - 60 - ui->btnClose->height(),
                              ui->btnClose->width(), ui->btnClose->height());

    ui->btnVisitWeb->setGeometry(this->width() - ui->btnVisitWeb->width() - 40, this->height() - 60 - ui->btnVisitWeb->height(),
                              ui->btnVisitWeb->width(), ui->btnVisitWeb->height());

    ui->chkBoxDisplay->setGeometry(40, this->height() - 60 - ui->chkBoxDisplay->height(),
                              ui->chkBoxDisplay->width(), ui->chkBoxDisplay->height());
}
