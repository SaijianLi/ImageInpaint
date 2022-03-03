#include "taomain.h"
#include "ui_taomain.h"

TaoMain::TaoMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TaoMain)
{
    // check if the network is connected
//    QHostInfo::lookupHost("www.taoshuiyin.com", this, SLOT(onLookupHost(QHostInfo)));

    viewerWindow.setAttribute(Qt::WA_QuitOnClose, false);

    ui->setupUi(this);

    this->setWindowIcon(QIcon(tr(".//icons//Eraser_32px.ico")));

    setAcceptDrops(true);

    createMenus();

    createToolBars();

//    scrollArea = new QScrollArea();
    scrollArea = new QScrollArea(this);

    canvasArea = new TaoCanvasArea(scrollArea);
    canvasArea->setBackgroundRole(QPalette::Base);
    canvasArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollArea->setVisible(true);
    scrollArea->setWidgetResizable(true);

    scrollArea->setWidget(canvasArea);
    scrollArea->setAlignment(Qt::AlignCenter);

    this->setCentralWidget(scrollArea);

//    this->setCentralWidget(canvasArea);

    setDrawingToolBarsVisible(false);

    setMainButtonsEnable(false);

//    // display help center
//    if (TaoSettings::isDisplayToolHelp("DisplayHelpCenterOnStart"))
//    {
//        helpTmpl.setToolName("DisplayHelpCenterOnStart");
//        helpTmpl.setHelpContent(QString("帮助中心"), "./help/index.html");
//        helpTmpl.setHiddenCheckBoxDisplay(false);
//        helpTmpl.show();
//    }


    // check if the exe file has been modified by hacker
//    QFile filePtr("TaoShuiYin.exe");

//    //QMessageBox::information(this, " file size : ", QString("%1").arg((int)filePtr.size()));

//    QCryptographicHash cryptoHash(QCryptographicHash::Md5);

//    QByteArray byteArrayExeMd5;

//    filePtr.open(QIODevice::ReadOnly);

//    QByteArray fileData = filePtr.read(10240);  // 10K

//    cryptoHash.reset();

//    while (!fileData.isEmpty())
//    {
//        cryptoHash.addData(fileData);
//        fileData = filePtr.read(10240);
//    }

//    filePtr.close();

//    byteArrayExeMd5 = cryptoHash.result();

//    QMessageBox::information(this, " MD5" , QString(byteArrayExeMd5.toHex()));
//    //compare with local version file md5 (淘水印.exe)

//    QFile localVersionMd5("curVersionFileMd5.txt");

//    localVersionMd5.open(QIODevice::ReadOnly);

//    QByteArray localMd5Data = localVersionMd5.readAll();

//    QJsonDocument jsonDocLocalMd5 = QJsonDocument::fromJson(localMd5Data, Q_NULLPTR);

//    QJsonObject jsonObjLocalMd5  = jsonDocLocalMd5.object();

//    QJsonArray jsonArrayFiles = QJsonArray();

//    QJsonObject jsonObjFiles = jsonObjLocalMd5.value(QString(QObject::tr("files"))).toObject();

//    QString localMd5 = jsonObjFiles.value(QObject::tr("TaoShuiYin.exe")).toString();

//    if (localMd5.compare(QString(byteArrayExeMd5.toHex())) != 0)
//    {
//        QMessageBox::about(NULL, QString("请重新下载淘水印"), QString("程序不是官方发出的，请去360软件管家\n或官网www.taoshuiyin.com重新下载淘水印！"));
//    }


    // download the server Version file md5, check version number.
    // compare with the local version, if not latest, remind to update
//    updateVersion = new TaoDownloadFile(this);

//    updateVersion->download(qstrUrlFileMd5);

//    registerUser = new TaoRegister();

//    registerUser->registerUser("12345690");
}


TaoMain::~TaoMain()
{
    delete ui;

    delete btnOpen;
    delete btnReload;
    delete btnSave;
    delete btnRedo;
    delete btnUndo;
    delete btnView;
    delete btnZoomIn;
    delete btnZoomOut;
    delete btnZoomOne;

    delete btnRadioPatch;
    delete btnRadioDrift;
    delete btnRadioLine;

    delete btnHelp;

    delete canvasArea;
    delete scrollArea;

    delete btnPatchClear;
    delete btnPatchCircleWatermark;
    delete btnRunPatchCompletionBlend;
    delete btnRunPatchCompletionCopy;

    delete btnDriftClear;
    delete btnSmallBrush;
    delete btnMediumBrush;
    delete btnBigBrush;
    delete btnTextureCircleBkgd;
    delete btnRunTextureCompletion;

    delete btnLineClear;
    delete btnLineDraw;
    delete btnRunLineCompletion;

    delete actOpenFile;
    delete actOpenCurDir;
    delete actSave;
    delete actReload;
    delete actExit;

    delete actClear;
    delete actUndo;
    delete actRedo;
    delete actPatchTool;
    delete actDriftTool;
    delete actLineTool;

    delete actZoomIn;
    delete actZoomOut;
    delete actZoomOne;
    delete actZoomTwo;
    delete actZoomThree;
    delete actZoomFour;

    delete actHelpCenter;
    delete actAboutUs;

    canvasArea->releaseMem();
}


//void TaoMain::onLookupHost(QHostInfo host)
//{
//    if (host.error() != QHostInfo::NoError) {
//        qDebug() << "Lookup failed:" << host.errorString();
//        //网络未连接，发送信号通知
////        QMessageBox::about(NULL, "网络问题", "请检查你的网络是否连接? 重试一次！");

//    }
//    else{
////        QMessageBox::about(NULL, "host info", "true");
//    }
//}

void TaoMain::exitClicked()
{
    if (blnSaved)
    {
        viewerWindow.close();

        helpTmpl.close();

        close();
    }
    else {
        switch(QMessageBox::warning(this, "保存已修改图片", "图片已修改，未保存！", "不退出", "直接退出", 0, 1))
        {
            case 1:
            viewerWindow.close();

            close();

            break;

        case 0:
        default:
            ;
        }
    }
}


void TaoMain::closeEvent(QCloseEvent *event)
{
    if (blnSaved)
    {
        event->accept();
        viewerWindow.close();

        helpTmpl.close();
    }
    else {
        switch(QMessageBox::warning(this, "保存已修改图片", "图片已修改，未保存！", "不退出", "直接退出", 0, 1))
        {
        case 1:
            event->accept();
            viewerWindow.close();

            break;

        case 0:
        default:
            event->ignore();
        }
    }
}


void TaoMain::dragEnterEvent(QDragEnterEvent *event)
{
//    QMessageBox::information(this, "drag", "drag event");

//    QStringList list = event->mimeData()->formats();

//    for (int icount = 0; icount < list.size(); icount++)
//    {
//        QString curFormat = list.at(icount);
//        cout << "format: " << curFormat.toStdString().c_str() << endl;
//    }

    if (event->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileNameW\""))
        event->accept();
}


void TaoMain::dropEvent(QDropEvent *event)
{
    if (!blnSaved)
    {
        switch(QMessageBox::warning(this, "保存已修改图片", "图片已修改，未保存！", "不打开", "打开新图片", 0, 1))
        {
        case 0:
            return;

        case 1:
        default:
            ;
        }
    }

    QByteArray mimeFilename;

    mimeFilename = event->mimeData()->data("application/x-qt-windows-mime;value=\"FileNameW\"");

    QString unicodeFilename = QTextCodec::codecForName("utf-16")->toUnicode(mimeFilename);

//    cout << " filename 1: " << filename.toStdString().data() << endl;

    QTextCodec *txtCodec = QTextCodec::codecForName("System");

    QTextEncoder encoder(txtCodec);

    QByteArray sysFilename = encoder.fromUnicode(unicodeFilename);

//    cout << " filename 2: " << sysFilename.toStdString().c_str() << endl;

    // make a QString, (if not the String code is not accepted in setting windowtitle
    inputFileName = QString(unicodeFilename.toStdString().c_str());

    // open file
    QFileInfo fileInfo(inputFileName);

    strQFileAbsolutePath = fileInfo.absolutePath();
    strQFileCompleteBaseName = fileInfo.completeBaseName();
    strQFileSuffix = fileInfo.suffix();

    strQCurBasePath = strQFileAbsolutePath;

//    QMessageBox::about(this, "drop filename", inputFileName);

//        cout << strQFileAbsolutePath.toStdString() << " " << strQFileCompleteBaseName.toStdString() << " " << strQFileSuffix.toStdString() << endl;

    viewerWindow.close();

    int iResultOpen;

    iResultOpen = canvasArea->openImage(inputFileName);

    switch (iResultOpen)
    {
    case 0:
            this->setWindowTitle(inputFileName + "  *未修改*");

//                scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize());
            scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                                 canvasArea->getDisplayImageSize().height() + displayMargin);

            lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
            // add 0.005, because float computation with 100 might get 139, 179.

            setMainButtonsEnable(true);

            // reset the current image is saved
            blnSaved = true;

            // clear all drawing variables
            canvasArea->patchClearDrawingVars();
            canvasArea->driftClearDrawingVars();

            if (!btnRadioPatch->isChecked() && !btnRadioDrift->isChecked()
                    && !btnRadioLine->isChecked())
            {
                btnRadioPatch->setChecked(true);
                patchToolClicked();
            }

        break;

    case 1:
        QMessageBox::about(this, "打开图片失败", "请检查照片是否存在和完整，图片格式为：jpg,jpeg,bmp,tif,tiff");
        break;
    case 2:
        QMessageBox::about(this, "打开图片失败", "请使用RGB格式图片");
        break;
    default:
        ;
    }
}


void TaoMain::createMenus()
{
    ui->menuBar->setDefaultUp(false);

    // for file menu
    menuFile = ui->menuFile;

    actOpenFile = new QAction("打开图片(&O)", this);
    actOpenCurDir = new QAction("打开所在文件夹(&F)", this);
    actViewImage = new QAction("查看原图(&V)", this);
    actReload = new QAction("重新开始(&R)", this);
    actSave = new QAction("保存图片(&S)", this);
    actExit = new QAction("退出(&X)", this);

    actOpenFile->setShortcut(Qt::CTRL + Qt::Key_O);
    actOpenCurDir->setShortcut(Qt::CTRL + Qt::Key_F);
    actViewImage->setShortcut(Qt::CTRL + Qt::Key_V);
    actReload->setShortcut(Qt::Key_F5);  //Qt::CTRL + Qt::Key_R
    actSave->setShortcut(Qt::CTRL + Qt::Key_S);
    actExit->setShortcut(Qt::CTRL + Qt::Key_Q);

    menuFile->addAction(actOpenFile);
    menuFile->addAction(actOpenCurDir);
    menuFile->addAction(actViewImage);
    menuFile->addAction(actReload);

    menuFile->addSeparator();
    menuFile->addAction(actSave);

    menuFile->addSeparator();
    menuFile->addAction(actExit);

    menuFile->setDefaultAction(actOpenFile);

    connect(actOpenFile, &QAction::triggered, this, &openFileClicked);
    connect(actOpenCurDir, &QAction::triggered, this, &openCurDirClicked);
    connect(actViewImage, &QAction::triggered, this, &viewClicked);
    connect(actReload, &QAction::triggered, this, &reloadClicked);
    connect(actSave, &QAction::triggered, this, &saveClicked);
    connect(actExit, &QAction::triggered, this, &exitClicked);

    // for edit menu
    menuEdit = ui->menuEdit;

    actClear = new QAction("清除选择(&C)", this);
    actUndo = new QAction("撤消(&U)", this);
    actRedo = new QAction("重做(&R)", this);
    actDriftTool = new QAction("漂移法(&I)", this);
    actPatchTool = new QAction("补衣服法(&P)", this);
    actLineTool = new QAction("线条修复(&L)", this);

    actClear->setShortcut(Qt::Key_Escape);
    actUndo->setShortcut(Qt::CTRL + Qt::Key_Z);
    actRedo->setShortcut(Qt::CTRL + Qt::Key_Y);
    actDriftTool->setShortcut(Qt::CTRL + Qt::Key_I);
    actPatchTool->setShortcut(Qt::CTRL + Qt::Key_P);
    actLineTool->setShortcut(Qt::CTRL + Qt::Key_L);

    menuEdit->addAction(actClear);
    menuEdit->addSeparator();
    menuEdit->addAction(actUndo);
    menuEdit->addAction(actRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actDriftTool);
    menuEdit->addAction(actPatchTool);
    menuEdit->addAction(actLineTool);

    menuEdit->setDefaultAction(actClear);

    connect(actClear, &QAction::triggered, this, &clearClicked);
    connect(actUndo, &QAction::triggered, this, &undoClicked);
    connect(actRedo, &QAction::triggered, this, &redoClicked);
    connect(actDriftTool, &QAction::triggered, this, &driftToolClicked);
    connect(actPatchTool, &QAction::triggered, this, &patchToolClicked);
    connect(actLineTool, &QAction::triggered, this, &lineToolClicked);

    // for view menu
    menuView = ui->menuView;

    actZoomIn = new QAction("放大图片(&=)", this); //&=  &I
    actZoomOut = new QAction("缩小图片(&-)", this); // &- &O
    actZoomOne = new QAction("原图大小(&1)", this);
    actZoomTwo = new QAction("2倍大小(&2)", this);
    actZoomThree = new QAction("3倍大小(&3)", this);
    actZoomFour = new QAction("4倍大小(&4)", this);

    actZoomIn->setShortcut(Qt::CTRL + Qt::Key_Equal);  // Equal, Up, PageUp
    actZoomOut->setShortcut(Qt::CTRL + Qt::Key_Minus);  // Minus, Down, PageDown
    actZoomOne->setShortcut(Qt::CTRL + Qt::Key_1);
    actZoomTwo->setShortcut(Qt::CTRL + Qt::Key_2);
    actZoomThree->setShortcut(Qt::CTRL + Qt::Key_3);
    actZoomFour->setShortcut(Qt::CTRL + Qt::Key_4);

    menuView->addAction(actZoomIn);
    menuView->addAction(actZoomOut);

    menuView->addSeparator();
    menuView->addAction(actZoomOne);
    menuView->addAction(actZoomTwo);
    menuView->addAction(actZoomThree);
    menuView->addAction(actZoomFour);

    menuView->setDefaultAction(actZoomTwo);

    connect(actZoomIn, &QAction::triggered, this, &zoomInClicked);
    connect(actZoomOut, &QAction::triggered, this, &zoomOutClicked);
    connect(actZoomOne, &QAction::triggered, this, &zoomOneClicked);
    connect(actZoomTwo, &QAction::triggered, this, &zoomTwoClicked);
    connect(actZoomThree, &QAction::triggered, this, &zoomThreeClicked);
    connect(actZoomFour, &QAction::triggered, this, &zoomFourClicked);

    // for help menu
    menuHelp = ui->menuHelp;

    actHelpCenter = new QAction("帮助中心(&H)", this);
    actAboutUs = new QAction("关于我们(&A)", this);

    QString curVersion = TaoSettings::getLocalVersion();
    actVersion = new QAction("当前版本 V" + curVersion, this);

    menuHelp->addAction(actHelpCenter);
    menuHelp->addAction(actAboutUs);
    menuHelp->addAction(actVersion);

    menuHelp->setDefaultAction(actHelpCenter);

    actHelpCenter->setShortcut(Qt::Key_F1); //Qt::CTRL + Qt::Key_H

    connect(actHelpCenter, &QAction::triggered, this, &helpClicked);
    connect(actAboutUs, &QAction::triggered, this, &aboutUsClicked);
}


void TaoMain::createToolBars()
{
    mainToolBar = ui->mainToolBar;
    driftToolBar = ui->driftToolBar;
    patchToolBar = ui->patchToolBar;
    lineToolBar = ui->lineToolBar;
    helpToolBar = ui->helpToolBar;

//    addToolBar(Qt::TopToolBarArea, mainToolBar)

    mainToolBar->setFixedHeight(40);
    patchToolBar->setFixedHeight(40);
    driftToolBar->setFixedHeight(40);
    lineToolBar->setFixedHeight(40);
    helpToolBar->setFixedHeight(40);

    mainToolBar->setIconSize(QSize(32, 32));
    patchToolBar->setIconSize(QSize(32, 32));
    driftToolBar->setIconSize(QSize(32, 32));
    lineToolBar->setIconSize(QSize(32, 32));
    helpToolBar->setIconSize(QSize(32, 32));

    // ******* main toolbar
    mainToolBar->setFont(QFont(QGuiApplication::font().family(), 12));
    // open image file
    btnOpen = new QToolButton(this);
    QIcon iconOpen(tr(".//icons//Folder_Open_32px.ico"));

    btnOpen->setStyleSheet("color : black");
    btnOpen->setIcon(iconOpen);
    btnOpen->setToolTip(tr("打开图片, 快捷键: Ctrl+O"));
    btnOpen->setAutoRaise(true);

    mainToolBar->addWidget(btnOpen);
    connect(btnOpen, SIGNAL(clicked(bool)), this, SLOT(openFileClicked()));

    // save image file
    btnSave = new QToolButton(this);
    QIcon iconSave(tr(".//icons//Save_32px.ico"));

    btnSave->setStyleSheet("color : black");
    btnSave->setIcon(iconSave);
    btnSave->setToolTip(tr("保存图片, 快捷键: Ctrl+S"));
    btnSave->setAutoRaise(true);
    mainToolBar->addWidget(btnSave);
    connect(btnSave, SIGNAL(clicked(bool)), this, SLOT(saveClicked()));

    // view original image
    btnView = new QToolButton(this);
    QIcon iconView(tr(".//icons//View_32px.ico"));

    btnView->setStyleSheet("color : black");
    btnView->setIcon(iconView);
    btnView->setToolTip(tr("查看原图, 快捷键: Ctrl+V"));
    btnView->setAutoRaise(true);

    mainToolBar->addWidget(btnView);
    connect(btnView, SIGNAL(clicked(bool)), this, SLOT(viewClicked()));

    mainToolBar->addSeparator();

    // reload image file
    btnReload = new QToolButton(this);
    QIcon iconReload(tr(".//icons//Reload_32px.ico"));

    btnReload->setIcon(iconReload);
    btnReload->setStyleSheet("color : black;");
    btnReload->setToolTip(tr("重新开始, 快捷键: F5"));
    btnReload->setAutoRaise(true);
    mainToolBar->addWidget(btnReload);
    connect(btnReload, SIGNAL(clicked(bool)), this, SLOT(reloadClicked()));

    // undo,
    btnUndo = new QToolButton(this);
    QIcon iconUndo(tr(".//icons//Undo_32px.ico"));

    btnUndo->setIcon(iconUndo);
    btnUndo->setStyleSheet("color : black");
    btnUndo->setToolTip(tr("撤消, 快捷键: Ctrl+Z"));
    btnUndo->setAutoRaise(true);
    mainToolBar->addWidget(btnUndo);
    connect(btnUndo, SIGNAL(clicked(bool)), this, SLOT(undoClicked()));

    // redo, implement in the future
    btnRedo = new QToolButton(this);
    QIcon iconRedo(tr(".//icons//Redo_32px.ico"));

    btnRedo->setIcon(iconRedo);

    btnRedo->setStyleSheet("color : black");
    btnRedo->setToolTip(tr("重做, 快捷键: Ctrl+Y"));
    btnRedo->setAutoRaise(true);
    mainToolBar->addWidget(btnRedo);
    connect(btnRedo, SIGNAL(clicked(bool)), this, SLOT(redoClicked()));

    mainToolBar->addSeparator();

    // zoom out
    btnZoomOut = new QToolButton(this);
    QIcon iconZoomOut(tr(".//icons//ZoomOut_32px.ico"));

    btnZoomOut->setStyleSheet("color : black");
    btnZoomOut->setIcon(iconZoomOut);
    btnZoomOut->setToolTip(tr("缩小图片, 快捷键: Ctrl+-"));
    btnZoomOut->setAutoRaise(true);
    mainToolBar->addWidget(btnZoomOut);
    connect(btnZoomOut, SIGNAL(clicked(bool)), this, SLOT(zoomOutClicked()));

    btnZoomIn = new QToolButton(this);
    QIcon iconZoomIn(tr(".//icons//ZoomIn_32px.ico"));

    btnZoomIn->setStyleSheet("color : black");
    btnZoomIn->setIcon(iconZoomIn);
    btnZoomIn->setToolTip(tr("放大图片, 快捷键: Ctrl+="));
    btnZoomIn->setAutoRaise(true);
    mainToolBar->addWidget(btnZoomIn);
    connect(btnZoomIn, SIGNAL(clicked(bool)), this, SLOT(zoomInClicked()));

    btnZoomOne = new QToolButton(this);
    QIcon iconZoomOne(tr(".//icons//ZoomOne_32px.ico"));

    btnZoomOne->setStyleSheet("color : black");
    btnZoomOne->setIcon(iconZoomOne);
    btnZoomOne->setToolTip(tr("原图大小, 快捷键: Ctrl+1\n2倍大小快捷键: Ctrl+2\n3倍大小快捷键: Ctrl+3"));
    btnZoomOne->setAutoRaise(true);
    mainToolBar->addWidget(btnZoomOne);
    connect(btnZoomOne, SIGNAL(clicked(bool)), this, SLOT(zoomOneClicked()));

    lblScale.setBaseSize(40, 32);
    lblScale.setStyleSheet("color : black");
    lblScale.setToolTip("当前比例");
    lblScale.setFont(QFont(QGuiApplication::font().family(), 14));
    mainToolBar->addWidget(&lblScale);

    mainToolBar->addSeparator();


    // for select inpainting method

    btnRadioDrift = new QRadioButton("漂移 ", this);
    btnRadioDrift->setStyleSheet("color : black");
    btnRadioDrift->setToolTip("漂移法, 快捷键: Ctrl+I\n提示：移动绿色框边缘，调整框大小");
    btnRadioDrift->setFont(QFont(QGuiApplication::font().family(), 11, 60));
    mainToolBar->addWidget(btnRadioDrift);
    connect(btnRadioDrift, SIGNAL(clicked(bool)), this, SLOT(driftToolClicked()));

    btnRadioPatch = new QRadioButton("补衣服 ", this);
    btnRadioPatch->setStyleSheet("color : black");
    btnRadioPatch->setToolTip("补衣服法, 快捷键: Ctrl+P\n提示：移动绿色框到吻合的位置");
    btnRadioPatch->setFont(QFont(QGuiApplication::font().family(), 11, 60));
    mainToolBar->addWidget(btnRadioPatch);
    connect(btnRadioPatch, SIGNAL(clicked(bool)), this, SLOT(patchToolClicked()));

    btnRadioLine = new QRadioButton("线条", this);
    btnRadioLine->setStyleSheet("color: black");
    btnRadioLine->setToolTip("修复破损的线条, 快捷键: Ctrl+L\n提示: 画需要修复线条");
    btnRadioLine->setFont(QFont(QGuiApplication::font().family(), 11, 60));
    mainToolBar->addWidget(btnRadioLine);
    connect(btnRadioLine, SIGNAL(clicked(bool)), this, SLOT(lineToolClicked()));

    // ******** for patch-based completion
    // clear selections
    btnPatchClear = new QToolButton(this);

    QIcon iconPatchClear(tr(".//icons//Clear_32px.ico"));

    btnPatchClear->setIconSize(QSize(32, 32));
    btnPatchClear->setIcon(iconPatchClear);
    btnPatchClear->setToolTip(tr("清除选择, 快捷键: Esc"));
    btnPatchClear->setAutoRaise(true);
    patchToolBar->addWidget(btnPatchClear);
    connect(btnPatchClear, SIGNAL(clicked(bool)), this, SLOT(patchClearClicked()));

    btnPatchCircleWatermark = new QToolButton(this);
    QIcon iconCircle(tr(".//icons//Lasso_32px.ico"));

    btnPatchCircleWatermark->setIconSize(QSize(32, 32));
    btnPatchCircleWatermark->setIcon(iconCircle);
    btnPatchCircleWatermark->setToolTip(tr("提示: 按住鼠标左键，圈选修补块周围大一点的区域"));
    btnPatchCircleWatermark->setAutoRaise(true);
    patchToolBar->addWidget(btnPatchCircleWatermark);
    connect(btnPatchCircleWatermark, SIGNAL(clicked(bool)), this, SLOT(patchCircleWatermarkClicked()));

    // process patch-tool completion
    btnRunPatchCompletionBlend = new QToolButton(this);
    QIcon iconPatchToolRun(tr(".//icons//Play_32px.ico"));

    btnRunPatchCompletionBlend->setFont(QFont(QGuiApplication::font().family(), 11, 60));

    btnRunPatchCompletionBlend->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnRunPatchCompletionBlend->setIconSize(QSize(32, 32));
    btnRunPatchCompletionBlend->setStyleSheet("color: black;");
    btnRunPatchCompletionBlend->setIcon(iconPatchToolRun);
    btnRunPatchCompletionBlend->setText("融合");
    btnRunPatchCompletionBlend->setToolTip(tr("融合绿色块到红色块的背景"));
    btnRunPatchCompletionBlend->setAutoRaise(true);
    patchToolBar->addWidget(btnRunPatchCompletionBlend);
    connect(btnRunPatchCompletionBlend, SIGNAL(clicked(bool)), this, SLOT(runPatchCompletionBlendClicked()));

    patchToolBar->addSeparator();

    btnRunPatchCompletionCopy = new QToolButton(this);
    QIcon iconPatchToolRun2(tr(".//icons//Play_32px.ico"));

    btnRunPatchCompletionCopy->setFont(QFont(QGuiApplication::font().family(), 11, 60));

    btnRunPatchCompletionCopy->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnRunPatchCompletionCopy->setStyleSheet("color: black");
    btnRunPatchCompletionCopy->setIcon(iconPatchToolRun2);
    btnRunPatchCompletionCopy->setText("复制");
    btnRunPatchCompletionCopy->setToolTip(tr("复制绿色块到红色块"));
    btnRunPatchCompletionCopy->setAutoRaise(true);
    patchToolBar->addWidget(btnRunPatchCompletionCopy);
    connect(btnRunPatchCompletionCopy, SIGNAL(clicked(bool)), this, SLOT(runPatchCompletionCopyClicked()));


    // for texture toolbar
    // clear selections
    btnDriftClear = new QToolButton(this);

    QIcon iconClear(tr(".//icons//Clear_32px.ico"));

    btnDriftClear->setIcon(iconClear);
    btnDriftClear->setToolTip(tr("清除选择, 快捷键: Esc"));
    btnDriftClear->setAutoRaise(true);
    driftToolBar->addWidget(btnDriftClear);
    connect(btnDriftClear, SIGNAL(clicked(bool)), this, SLOT(driftClearClicked()));

    // brush_small watermark
    btnSmallBrush = new QToolButton(this);
    QIcon iconSmallBrush(tr(".//icons//Brush_16px.ico"));
    btnSmallBrush->setStyleSheet(" color: black;");
    btnSmallBrush->setIcon(iconSmallBrush);
    btnSmallBrush->setToolTip(tr("提示: 1.按住鼠标左键,小笔涂擦水印,\n   2.调整绿色框大小"));
    btnSmallBrush->setAutoRaise(true);
    driftToolBar->addWidget(btnSmallBrush);
    connect(btnSmallBrush, SIGNAL(clicked(bool)), this, SLOT(smallBrushClicked()));

    // brush_medium watermark
    btnMediumBrush = new QToolButton(this);
    QIcon iconMediumBrush(tr(".//icons//Brush_24px.ico"));

    btnMediumBrush->setStyleSheet(" color: black;");
    btnMediumBrush->setIcon(iconMediumBrush);
    btnMediumBrush->setToolTip(tr("提示:1. 按住鼠标左键,中笔涂擦水印,\n    2.调整绿色框大小"));
    btnMediumBrush->setAutoRaise(true);
    driftToolBar->addWidget(btnMediumBrush);
    connect(btnMediumBrush, SIGNAL(clicked(bool)), this, SLOT(mediumBrushClicked()));

    // brush_big watermark
    btnBigBrush = new QToolButton(this);
    QIcon iconBigBrush(tr(".//icons//Brush_32px.ico"));

    btnBigBrush->setStyleSheet(" color: black;");
    btnBigBrush->setIcon(iconBigBrush);
    btnBigBrush->setToolTip(tr("提示：1. 按住鼠标左键,粗笔涂擦水印, \n    2.调整绿色框大小"));
    btnBigBrush->setAutoRaise(true);
    driftToolBar->addWidget(btnBigBrush);
    connect(btnBigBrush, SIGNAL(clicked(bool)), this, SLOT(bigBrushClicked()));

    btnTextureCircleBkgd = new QToolButton(this);
    QIcon iconCircleBkgd(tr(".//icons//Lasso_32px.ico"));
    btnTextureCircleBkgd->setIcon(iconCircleBkgd);
    btnTextureCircleBkgd->setToolTip(tr("（可选）按住鼠标左键，圈选填充的背景"));
    btnTextureCircleBkgd->setAutoRaise(true);
//    driftToolBar->addWidget(btnTextureCircleBkgd);
    connect(btnTextureCircleBkgd, SIGNAL(clicked(bool)), this, SLOT(textureCircleBkgdClicked()));

    btnTextureCircleBkgd->setHidden(true);

    lblTextureCircleBkgd.setBaseSize(40, 32);
    lblTextureCircleBkgd.setFont(QFont(QGuiApplication::font().family(), 10));
    lblTextureCircleBkgd.setText("圈选背景 ");
//    driftToolBar->addWidget(&lblTextureCircleBkgd);

    lblTextureCircleBkgd.setHidden(true);

    driftToolBar->addSeparator();

    // run texture-based completion
    btnRunTextureCompletion = new QToolButton(this);
    QIcon iconPlay(tr(".//icons//Play_32px.ico"));

    btnRunTextureCompletion->setIcon(iconPlay);
    btnRunTextureCompletion->setToolTip(tr("运行纹理修复"));
    btnRunTextureCompletion->setAutoRaise(true);
//    driftToolBar->addWidget(btnRunTextureCompletion);
    connect(btnRunTextureCompletion, SIGNAL(clicked(bool)), this, SLOT(runTextureCompletionClicked()));

    btnRunTextureCompletion->setHidden(true);

    // run Drift completion
    btnRunDriftCompletion = new QToolButton(this);
    QIcon iconDriftRun(tr(".//icons//Play_32px.ico"));

    btnRunDriftCompletion->setFont(QFont(QGuiApplication::font().family(), 11, 60));

    btnRunDriftCompletion->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    btnRunDriftCompletion->setStyleSheet(" color: black;");
    btnRunDriftCompletion->setIcon(iconDriftRun);

    btnRunDriftCompletion->setText("运行");
    btnRunDriftCompletion->setToolTip(tr("运行漂移修复"));
    btnRunDriftCompletion->setAutoRaise(true);
    driftToolBar->addWidget(btnRunDriftCompletion);
    connect(btnRunDriftCompletion, SIGNAL(clicked(bool)), this, SLOT(runDriftCompletionClicked()));


    // for line completion
    // clear selections
    btnLineClear = new QToolButton(this);

    QIcon iconLineClear(tr(".//icons//Clear_32px.ico"));

    btnLineClear->setStyleSheet(" color: black;");
    btnLineClear->setIcon(iconLineClear);
    btnLineClear->setToolTip(tr("清除选择, 快捷键: Esc"));
    btnLineClear->setAutoRaise(true);
    lineToolBar->addWidget(btnLineClear);
    connect(btnLineClear, SIGNAL(clicked(bool)), this, SLOT(lineClearClicked()));

    btnLineDraw = new QToolButton(this);
    QIcon iconDrawLine(tr(".//icons//LineMark_32px.ico"));   //.//icons//Drag_32px.png

    btnLineDraw->setStyleSheet("color: black;");
    btnLineDraw->setIcon(iconDrawLine);
    btnLineDraw->setToolTip(tr("提示：按住鼠标左键，画修复的线条"));
    btnLineDraw->setAutoRaise(true);
    lineToolBar->addWidget(btnLineDraw);
    connect(btnLineDraw, SIGNAL(clicked(bool)), this, SLOT(lineDrawClicked()));

    // process line completion
    btnRunLineCompletion = new QToolButton(this);
    QIcon iconlineToolRun(tr(".//icons//Play_32px.ico"));

    btnRunLineCompletion->setFont(QFont(QGuiApplication::font().family(), 11, 60));

    btnRunLineCompletion->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    btnRunLineCompletion->setStyleSheet(" color : black; ");
    btnRunLineCompletion->setIcon(iconlineToolRun);
    btnRunLineCompletion->setText("运行");
    btnRunLineCompletion->setToolTip(tr("运行修复线条"));
    btnRunLineCompletion->setAutoRaise(true);
    lineToolBar->addWidget(btnRunLineCompletion);
    connect(btnRunLineCompletion, SIGNAL(clicked(bool)), this, SLOT(runLineCompletionClicked()));

    // help
    btnHelp = new QToolButton(this);
    QIcon iconHelp(tr(".//icons//Help_32px.ico"));

    btnHelp->setFont(QFont(QGuiApplication::font().family(), 11, 60));

    btnHelp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    btnHelp->setStyleSheet(" color: black");
    btnHelp->setIcon(iconHelp);
    btnHelp->setText("帮助");
    btnHelp->setToolTip(tr("帮助中心, 快捷键: F1"));
    btnHelp->setAutoRaise(true);
    helpToolBar->addWidget(btnHelp);
    connect(btnHelp, SIGNAL(clicked(bool)), this, SLOT(helpClicked()));
}


void TaoMain::patchToolClicked()
{
    if (inputFileName == "")
        return;

    btnRadioPatch->setChecked(true);

    patchToolBar->setVisible(true);
    driftToolBar->setVisible(false);
    lineToolBar->setVisible(false);

    setPatchButtonsEnable(true);
    setPatchButtonsCheckable(true);

    setPatchButtonsUnChecked();

    setDriftButtonsEnable(false);
    setLineButtonsEnable(false);

    canvasArea->setPatchToolsUnSelected();
    canvasArea->setDriftToolsUnSelected();
    canvasArea->setLineToolsUnSelected();

    canvasArea->unsetCursor();

    // set the default tool selected
    btnPatchCircleWatermark->setChecked(true);
    canvasArea->setPatchCircleWatermarkSelected(true);

    canvasArea->loadCurQImage();

    if (TaoSettings::isDisplayToolHelp("DisplayPatchHelpOnStart"))
    {
        helpTmpl.setToolName("DisplayPatchHelpOnStart");
        helpTmpl.setHelpContent(QString("补衣服法使用帮助"), "./help/patch.html");
        helpTmpl.setHiddenCheckBoxDisplay(false);
        helpTmpl.show();

//        QString("<a id=\"part1\">Part 1</a>圈住水印，移动绿色的圈圈到可以填补红色圈圈的地方，点击融合或复制，"
//                "确定最终效果。 <a href=\"http://www.baidu.com\">更多帮助...</a>"
//                "<a id=\"part2\">Part 2</a> 涂擦水印，移动绿色的圈圈到可以填补红色圈圈的地方，点击融合或复制，"
//                "确定最终效果。 <a href=\"#part1\"> part 1...</a>"));
    }
}


void TaoMain::driftToolClicked()
{
    if (inputFileName == "")
        return;

    btnRadioDrift->setChecked(true);

    driftToolBar->setVisible(true);
    patchToolBar->setVisible(false);
    lineToolBar->setVisible(false);

    setDriftButtonsEnable(true);
    setDriftButtonsCheckable(true);
    setDriftButtonsUnChecked();

    setPatchButtonsEnable(false);
    setLineButtonsEnable(false);

    canvasArea->setPatchToolsUnSelected();
    canvasArea->setDriftToolsUnSelected();
    canvasArea->setLineToolsUnSelected();

    canvasArea->unsetCursor();

    canvasArea->setPatchCompleted(false);

    // set the default tool selected
    btnMediumBrush->setChecked(true);
    canvasArea->setMediumBrushSelected(true);
    iBrushSelected = 10;

    canvasArea->loadCurQImage();

    if (TaoSettings::isDisplayToolHelp("DisplayDriftHelpOnStart"))
    {
        helpTmpl.setToolName("DisplayDriftHelpOnStart");
        helpTmpl.setHelpContent(QString("漂移法使用帮助"), "./help/drift.html");
        helpTmpl.setHiddenCheckBoxDisplay(false);
        helpTmpl.show();
    }
}


void TaoMain::lineToolClicked()
{
    if (inputFileName == "")
        return;

    btnRadioLine->setChecked(true);

    lineToolBar->setVisible(true);
    patchToolBar->setVisible(false);
    driftToolBar->setVisible(false);

    setLineButtonsEnable(true);
    setLineButtonsCheckable(true);
    setLineButtonsUnChecked();

    setPatchButtonsEnable(false);
    setDriftButtonsEnable(false);

    canvasArea->setPatchToolsUnSelected();
    canvasArea->setDriftToolsUnSelected();
    canvasArea->setLineToolsUnSelected();

    canvasArea->setPatchCompleted(false);

    canvasArea->unsetCursor();

    // set the default tool selected
    btnLineDraw->setChecked(true);
    canvasArea->setLineDrawSelected(true);

    canvasArea->loadCurQImage();

    if (TaoSettings::isDisplayToolHelp("DisplayLineHelpOnStart"))
    {
        helpTmpl.setToolName("DisplayLineHelpOnStart");
        helpTmpl.setHelpContent(QString("直线修复使用帮助"), "./help/line.html");
        helpTmpl.setHiddenCheckBoxDisplay(false);
        helpTmpl.show();
    }
}


void TaoMain::lineDrawClicked()
{
    canvasArea->setLineDrawSelected(true);
    btnLineDraw->setChecked(true);
}


void TaoMain::openCurDirClicked()
{
    if (strQFileAbsolutePath != "")
        QDesktopServices::openUrl(QUrl(strQFileAbsolutePath, QUrl::TolerantMode));
}


void TaoMain::openFileClicked()
{
    if (!blnSaved)
    {
        switch(QMessageBox::warning(this, "保存已修改图片", "图片已修改，未保存！", "不打开", "打开新图片", 0, 1))
        {
        case 0:
            return;

        case 1:
        default:
            ;
        }
    }

    inputFileName = QFileDialog::getOpenFileName(this, tr("Open Image"), strQCurBasePath, tr("*.jpg *.jpeg *.bmp *.tif *.tiff"));

//    QMessageBox::about(this,"filename", inputFileName);

    if (!inputFileName.isEmpty())
    {
        QFileInfo fileInfo(inputFileName);

        strQFileAbsolutePath = fileInfo.absolutePath();
        strQFileCompleteBaseName = fileInfo.completeBaseName();
        strQFileSuffix = fileInfo.suffix();

        strQCurBasePath = strQFileAbsolutePath;

//        cout << strQFileAbsolutePath.toStdString() << " " << strQFileCompleteBaseName.toStdString() << " " << strQFileSuffix.toStdString() << endl;

        viewerWindow.close();

        int iResultOpen;

        iResultOpen = canvasArea->openImage(inputFileName);

        switch (iResultOpen)
        {
        case 0:
                this->setWindowTitle(inputFileName + "  *未修改*");

//                scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize());
                scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                                     canvasArea->getDisplayImageSize().height() + displayMargin);

                lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
                // add 0.005, because float computation with 100 might get 139, 179.

                setMainButtonsEnable(true);

                // reset the current image is saved
                blnSaved = true;

//                canvasArea->unsetCursor();

                // clear all drawing variables
                canvasArea->patchClearDrawingVars();
                canvasArea->driftClearDrawingVars();

                if (!btnRadioPatch->isChecked() && !btnRadioDrift->isChecked()
                        && !btnRadioLine->isChecked())
                {
                    btnRadioDrift->setChecked(true);
                    driftToolClicked();
                }

//                setPatchButtonsUnChecked();
//                setDriftButtonsUnChecked();
//                setLineButtonsUnChecked();

//                canvasArea->setPatchToolsUnSelected();
//                canvasArea->setDriftToolsUnSelected();
//                canvasArea->setLineToolsUnSelected();

            break;

        case 1:
            QMessageBox::about(this, "打开图片失败", "请检查照片是否存在和完整，图片格式为：jpg,jpeg,bmp,tif,tiff");
            break;
        case 2:
            QMessageBox::about(this, "打开图片失败", "请使用RGB格式图片");
            break;
        default:
            ;
        }
    }
}


void TaoMain::saveClicked()
{
    if (inputFileName == "")
        return;

    if (blnSaved)
        return;

    wxPay = new TaoWxPay();
    connect(wxPay, SIGNAL(saveImage()), this, SLOT(saveImageFile()));

//    wxPay->httpSaveRequest(QString("http://www.taoshuiyin.com:8082/demo/index/wxpayexample1"), saveRequest);

//    wxPay->httpSaveRequest(QString("http://www.taoshuiyin.com/api/order/process/add/product_id/1/machine_id/4C94685E68EF6A9BD0D9865E"), saveRequest);

    QString guid = TaoSettings::readKeyValue("GUID");
    wxPay->httpSaveRequest(QString("http://www.taoshuiyin.com/api/product/process/request_save/product_id/1/machine_id/") + guid, saveRequest);
}


void TaoMain::saveImageFile()
{
    wxPay->close();

    QString tmpFileName = strQCurBasePath +"//" + strQFileCompleteBaseName + "_修复后" + "." + strQFileSuffix;

    savedFileName = QFileDialog::getSaveFileName(this, tr("Save Image"), tmpFileName, tr("*.jpg *.jpeg *.bmp *.tif *.tiff"));
    // strQCurBasePath

    if (!savedFileName.isEmpty())
    {
//        cout << savedFileName.toStdString() << endl;
        if (!(savedFileName.endsWith(".jpg", Qt::CaseInsensitive) ||
                savedFileName.endsWith(".jpeg", Qt::CaseInsensitive) ||
                savedFileName.endsWith(".bmp", Qt::CaseInsensitive)||
                savedFileName.endsWith(".tif", Qt::CaseInsensitive)||
                savedFileName.endsWith(".tiff", Qt::CaseInsensitive)))
        {
            savedFileName.append(".jpg");
        }

        canvasArea->saveImage(savedFileName); //, "");


        QFileInfo fileInfo(savedFileName);

        strQFileAbsolutePath = fileInfo.absolutePath();
        strQFileCompleteBaseName = fileInfo.completeBaseName();
        strQFileSuffix = fileInfo.suffix();
        strQCurBasePath = strQFileAbsolutePath;

        inputFileName = savedFileName;
        this->setWindowTitle(inputFileName);

        // reset the current image is saved
        blnSaved = true;

        // clear all drawing variables
        canvasArea->patchClearDrawingVars();
        canvasArea->driftClearDrawingVars();
        canvasArea->lineClearDrawingVars();

//        filePath.replace("/","\\"); //将地址中的"/"替换为"\"，因为在Windows下使用的是"\"

//        cout << filePath.toStdString() << endl;
//        QProcess::startDetached("explorer " + filePath); //打开上面获取的目录

        iCountSaved++;

        QDesktopServices::openUrl(QUrl::fromLocalFile(strQFileAbsolutePath));
    }
}


void TaoMain::reloadClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->reloadImage();

    this->setWindowTitle(inputFileName + "  *未修改*");

    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));

    // reset the current image is saved
    blnSaved = true;

    // clear all drawing variables
    canvasArea->patchClearDrawingVars();
    canvasArea->driftClearDrawingVars();

//    setMainButtonsEnable(true);

//    setPatchButtonsUnChecked();
//    setDriftButtonsUnChecked();
//    setLineButtonsUnChecked();

//    canvasArea->setPatchToolsUnSelected();
//    canvasArea->setDriftToolsUnSelected();
//    canvasArea->setLineToolsUnSelected();
}


void TaoMain::viewClicked()
{
    if (inputFileName == "")
        return;

    QImage inImage;

    canvasArea->getOrigImage(inImage);

    viewerWindow.setScalePos(canvasArea->getScalePos());
    viewerWindow.setImage(inImage);

    viewerWindow.setWindowTitle(tr("原图"));
    viewerWindow.show();
}


void TaoMain::undoClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->unsetCursor();
    canvasArea->undo();

    canvasArea->patchClearDrawingVars();
    canvasArea->driftClearDrawingVars();

//    canvasArea->setPatchToolsUnSelected();
//    canvasArea->setDriftToolsUnSelected();
//    canvasArea->setLineToolsUnSelected();
}


void TaoMain::redoClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->unsetCursor();
    canvasArea->redo();

    canvasArea->patchClearDrawingVars();
    canvasArea->driftClearDrawingVars();

//    canvasArea->setPatchToolsUnSelected();
//    canvasArea->setDriftToolsUnSelected();
//    canvasArea->setLineToolsUnSelected();
}


void TaoMain::smallBrushClicked()
{
    canvasArea->setSmallBrushSelected(true);
    setTextureButtonChecked(*btnSmallBrush);

    iBrushSelected = 1;
}


void TaoMain::mediumBrushClicked()
{
    canvasArea->setMediumBrushSelected(true);
    setTextureButtonChecked(*btnMediumBrush);

    iBrushSelected = 10;
}


void TaoMain::bigBrushClicked()
{
    canvasArea->setBigBrushSelected(true);
    setTextureButtonChecked(*btnBigBrush);

    iBrushSelected = 100;
}


void TaoMain::textureCircleBkgdClicked()
{
    canvasArea->setTextureCircleBkgdSelected(true);
    setTextureButtonChecked(*btnTextureCircleBkgd);
}


// clear only the drawings, not the tool selection
void TaoMain::driftClearClicked()
{
    canvasArea->driftClearImage();
}


void TaoMain::patchClearClicked()
{
    canvasArea->patchClearImage();    
}


void TaoMain::lineClearClicked()
{
    canvasArea->lineClearImage();
}


void TaoMain::zoomInClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomImage(1);
//    cout << canvasArea->getDisplayImageSize().width() << "  " << canvasArea->getDisplayImageSize().height() << endl;
//    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize());
    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::zoomOutClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomImage(-1);
//    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize());
    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::zoomOneClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomRatioImage(1);
//    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize());
    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::zoomTwoClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomRatioImage(2);

    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::zoomThreeClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomRatioImage(3);
    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::zoomFourClicked()
{
    if (inputFileName == "")
        return;

    canvasArea->zoomRatioImage(4);
    scrollArea->widget()->setMinimumSize(canvasArea->getDisplayImageSize().width(),
                                         canvasArea->getDisplayImageSize().height() + displayMargin);


    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + canvasArea->getCurScale()))));
}


void TaoMain::runPatchCompletionBlendClicked()
{
    if ((!canvasArea->isDrawnPatch() && !canvasArea->isPatchCompleted()) ||
        (canvasArea->isDrawnPatch() && canvasArea->isPatchCompleted()))
        return;

    QString titleStr = inputFileName + "   *未保存*";

    this->setWindowTitle(titleStr);

    processing = new TaoProcessing(0, "正在处理中...");

    processing->show();

    canvasArea->patchCompleteImageBlend();

    processing->close();

    delete processing;

    blnSaved = false;

//    canvasArea->setPatchToolsUnSelected();
//    setPatchButtonsUnChecked();
}


void TaoMain::runPatchCompletionCopyClicked()
{
    if ((!canvasArea->isDrawnPatch() && !canvasArea->isPatchCompleted()) ||
        (canvasArea->isDrawnPatch() && canvasArea->isPatchCompleted()))
        return;

    QString titleStr = inputFileName + "   *未保存*";
    this->setWindowTitle(titleStr);

    processing = new TaoProcessing(0, "正在处理中...");

    processing->show();

    canvasArea->patchCompleteImageCopy();

    processing->close();

    delete processing;

    blnSaved = false;

//    canvasArea->setPatchToolsUnSelected();
//    setPatchButtonsUnChecked();
}


void TaoMain::runTextureCompletionClicked()
{
    if (!canvasArea->isDrawnTexture())
        return;

    QString titleStr = inputFileName + "   *未保存*";
    this->setWindowTitle(titleStr);

    processing = new TaoProcessing(0, "正在处理中...");

    processing->show();

    int iCompleteResult = -1;

    if (canvasArea->isDrawnTextureBkgd())
    {
        iCompleteResult = canvasArea->textureCompleteImageManual();

        // if bkgd ROI Rect less than 2 * patchSize, remind user to draw bigger
        if (iCompleteResult == 3)
        {
            QMessageBox::information(this, "提醒", "绿色部分圈得太小了，请圈大点！", "我知道了", 0);

            processing->close();

            delete processing;

            return;
        }
    }
    else
    {
        canvasArea->textureCompleteImageAuto();
    }

    processing->close();

    delete processing;

    blnSaved = false;

//    canvasArea->setDriftToolsUnSelected();
//    setDriftButtonsUnChecked();

    if (btnTextureCircleBkgd->isChecked())
    {
        canvasArea->setTextureCircleBkgdSelected(false);
        btnTextureCircleBkgd->setChecked(false);

        switch (iBrushSelected)
        {
        case 1:
            canvasArea->setSmallBrushSelected(true);
            btnSmallBrush->setChecked(true);
            break;

        case 10:
            canvasArea->setMediumBrushSelected(true);
            btnMediumBrush->setChecked(true);
            break;

        case 100:
            canvasArea->setBigBrushSelected(true);
            btnBigBrush->setChecked(true);
            break;

        default:
            ;
        }
    }
}


void TaoMain::runDriftCompletionClicked()
{
    if (!canvasArea->isDrawnTexture())
        return;

    QString titleStr = inputFileName + "   *未保存*";
    this->setWindowTitle(titleStr);

    processing = new TaoProcessing(0, "正在处理中...");

    processing->show();

    int iCompleteResult = -1;

//    canvasArea->shiftMapCompleteImage();
    canvasArea->driftCompleteImage();

    processing->close();

    delete processing;

    blnSaved = false;

    if (btnTextureCircleBkgd->isChecked())
    {
        canvasArea->setTextureCircleBkgdSelected(false);
        btnTextureCircleBkgd->setChecked(false);

        switch (iBrushSelected)
        {
        case 1:
            canvasArea->setSmallBrushSelected(true);
            btnSmallBrush->setChecked(true);
            break;

        case 10:
            canvasArea->setMediumBrushSelected(true);
            btnMediumBrush->setChecked(true);
            break;

        case 100:
            canvasArea->setBigBrushSelected(true);
            btnBigBrush->setChecked(true);
            break;

        default:
            ;
        }
    }
}


void TaoMain::runLineCompletionClicked()
{
    if (!canvasArea->isDrawnEdge())
        return;

    QString titleStr = inputFileName + "   *未保存*";
    this->setWindowTitle(titleStr);

    canvasArea->lineCompleteImage();

    blnSaved = false;

//    canvasArea->setLineToolsUnSelected();
//    setLineButtonsUnChecked();
}


void TaoMain::helpClicked()
{
//    QDesktopServices::openUrl(QUrl(tr("www.baidu.com")));
//    helpWindow.show();

    helpTmpl.setToolName("DisplayHelpCenterOnStart");
    helpTmpl.setHelpContent(QString("帮助中心"), "./help/index.html");
    helpTmpl.setHiddenCheckBoxDisplay(true);
    helpTmpl.show();
}


void TaoMain::aboutUsClicked()
{
    helpTmpl.setToolName("");
    helpTmpl.setHelpContent(QString("关于我们"), "./help/aboutUs.html");
    helpTmpl.setHiddenCheckBoxDisplay(true);
    helpTmpl.show();
}


void TaoMain::clearClicked()
{
    if (inputFileName == "")
        return;

    if (btnRadioPatch->isChecked())
        patchClearClicked();
    else if (btnRadioDrift->isChecked())
        driftClearClicked();
    else
        lineClearClicked();
}


void TaoMain::patchCircleWatermarkClicked()
{
    canvasArea->setPatchCircleWatermarkSelected(true);
    btnPatchCircleWatermark->setChecked(true);
}


void TaoMain::setDrawingToolBarsVisible(bool visible)
{
    patchToolBar->setVisible(visible);
    driftToolBar->setVisible(visible);
    lineToolBar->setVisible(visible);
}


void TaoMain::setMainButtonsEnable(bool enable)
{
    btnSave->setEnabled(enable);
    btnReload->setEnabled(enable);
    btnView->setEnabled(enable);

    btnRedo->setEnabled(enable);
    btnUndo->setEnabled(enable);

    btnZoomIn->setEnabled(enable);
    btnZoomOut->setEnabled(enable);
    btnZoomOne->setEnabled(enable);

    btnRadioPatch->setEnabled(enable);
    btnRadioDrift->setEnabled(enable);
    btnRadioLine->setEnabled(enable);
}


void TaoMain::setDriftButtonsEnable(bool enable)
{
    btnDriftClear->setEnabled(enable);

    btnSmallBrush->setEnabled(enable);
    btnMediumBrush->setEnabled(enable);
    btnBigBrush->setEnabled(enable);

    btnTextureCircleBkgd->setEnabled(enable);

    btnRunTextureCompletion->setEnabled(enable);
}


void TaoMain::setPatchButtonsEnable(bool enable)
{
    btnPatchClear->setEnabled(enable);

    btnPatchCircleWatermark->setEnabled(enable);

    btnRunPatchCompletionBlend->setEnabled(enable);
    btnRunPatchCompletionCopy->setEnabled(enable);
}


void TaoMain::setLineButtonsEnable(bool enable)
{
    btnLineClear->setEnabled(enable);

    btnLineDraw->setEnabled(enable);

    btnRunLineCompletion->setEnabled(enable);
}


void TaoMain::setDriftButtonsCheckable(bool checkable)
{
    btnSmallBrush->setCheckable(checkable);
    btnMediumBrush->setCheckable(checkable);
    btnBigBrush->setCheckable(checkable);

    btnTextureCircleBkgd->setCheckable(checkable);
}


void TaoMain::setPatchButtonsCheckable(bool checkable)
{
    btnPatchCircleWatermark->setCheckable(checkable);
}


void TaoMain::setLineButtonsCheckable(bool checkable)
{
    btnLineDraw->setCheckable(checkable);
}


void TaoMain::setPatchButtonsUnChecked()
{
    btnPatchCircleWatermark->setChecked(false);
}


void TaoMain::setDriftButtonsUnChecked()
{
    btnSmallBrush->setChecked(false);
    btnMediumBrush->setChecked(false);
    btnBigBrush->setChecked(false);

    btnTextureCircleBkgd->setChecked(false);
}


void TaoMain::setLineButtonsUnChecked()
{
    btnLineDraw->setChecked(false);
}


void TaoMain::setTextureButtonChecked(QToolButton &inButton)
{
    btnSmallBrush->setChecked(false);
    btnMediumBrush->setChecked(false);
    btnBigBrush->setChecked(false);

    btnTextureCircleBkgd->setChecked(false);

    inButton.setChecked(true);
}


