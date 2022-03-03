

#ifndef TAOMAIN_H
#define TAOMAIN_H

#include <QMainWindow>
#include <QMessageBox>
#include <QAction>
#include <QToolBar>
#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QToolButton>
#include <QSpinBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <QImage>
#include <QImageReader>
#include <QSize>
#include <QScrollBar>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QStyleFactory>
#include <QWindow>
#include <QProcess>
#include <QUrl>
#include <QAction>
#include <QMenu>
#include <QMimeData>
#include <QTextCodec>
#include <QTextEncoder>
#include <QLibrary>
#include <QCryptographicHash>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkConfigurationManager>
#include <QtNetwork/QHostInfo>


#include "taocanvasarea.h"
#include "taoimageviewer.h"
#include "taoprocessing.h"
#include "taosettings.h"
#include "taohelptemplate.h"
#include "taodownloadfile.h"
#include "taowxpay.h"
#include "taoregister.h"

#include <cstdio>

using namespace std;

namespace Ui {
class TaoMain;
}


class TaoMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaoMain(QWidget *parent = 0);
    ~TaoMain();

protected:
    void closeEvent(QCloseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void openFileClicked();
    void openCurDirClicked();
    void saveClicked();
    void reloadClicked();
    void viewClicked();
    void undoClicked();
    void redoClicked();
    void zoomInClicked();
    void zoomOutClicked();
    void zoomOneClicked();
    void zoomTwoClicked();
    void zoomThreeClicked();
    void zoomFourClicked();
    void helpClicked();

    void exitClicked();
    void aboutUsClicked();

    void clearClicked();

    // for patch completion
    void patchToolClicked();
    void patchCircleWatermarkClicked();
    void patchClearClicked();
    void runPatchCompletionBlendClicked();
    void runPatchCompletionCopyClicked();

    // for texture completion
    void driftToolClicked();
    void smallBrushClicked();
    void mediumBrushClicked();
    void bigBrushClicked();
    void driftClearClicked();
    void runDriftCompletionClicked();

    void textureCircleBkgdClicked();
    void runTextureCompletionClicked();


    // for line completion
    void lineToolClicked();
    void lineDrawClicked();
    void lineClearClicked();
    void runLineCompletionClicked();

    void saveImageFile();

//    void onLookupHost(QHostInfo host);

private:
    Ui::TaoMain *ui;

    void createMenus();
    void createToolBars();

    void setDrawingToolBarsVisible(bool visible);

    void setMainButtonsEnable(bool enable);
    void setPatchButtonsEnable(bool enable);
    void setDriftButtonsEnable(bool enable);
    void setLineButtonsEnable(bool enable);

    void setPatchButtonsCheckable(bool checkable);
    void setDriftButtonsCheckable(bool checkable);
    void setLineButtonsCheckable(bool checkable);

    void setTextureButtonChecked(QToolButton &inButton);

    void setPatchButtonsUnChecked();
    void setDriftButtonsUnChecked();
    void setLineButtonsUnChecked();

    // actions for menu
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuHelp;

    QAction *actOpenFile;
    QAction *actOpenCurDir;
    QAction *actViewImage;
    QAction *actReload;
    QAction *actSave;
    QAction *actExit;

    QAction *actClear;
    QAction *actUndo;
    QAction *actRedo;
    QAction *actPatchTool;
    QAction *actDriftTool;
    QAction *actLineTool;

    QAction *actZoomIn;
    QAction *actZoomOut;
    QAction *actZoomOne;
    QAction *actZoomTwo;
    QAction *actZoomThree;
    QAction *actZoomFour;

    QAction *actHelpCenter;
    QAction *actAboutUs;
    QAction *actVersion;

    // buttons for main toolbar
    QToolBar *mainToolBar;
    QToolBar *patchToolBar;
    QToolBar *driftToolBar;
    QToolBar *lineToolBar;
    QToolBar *helpToolBar;

    QRadioButton *btnRadioPatch;
    QRadioButton *btnRadioDrift;
    QRadioButton *btnRadioLine;

    QToolButton *btnOpen;
    QToolButton *btnSave;
    QToolButton *btnReload;
    QToolButton *btnUndo;
    QToolButton *btnRedo;
    QToolButton *btnView;
    QToolButton *btnZoomIn;
    QToolButton *btnZoomOut;
    QToolButton *btnZoomOne;
    QToolButton *btnHelp;
    QLabel lblScale;

    // buttons for patch toolbar
    QToolButton *btnPatchClear;
    QToolButton *btnPatchCircleWatermark;
    QToolButton *btnRunPatchCompletionBlend;
    QToolButton *btnRunPatchCompletionCopy;

    // buttons for drift toolbar
    QToolButton *btnSmallBrush;
    QToolButton *btnMediumBrush;
    QToolButton *btnBigBrush;
    QToolButton *btnDriftClear;
    QToolButton *btnRunDriftCompletion;

    QToolButton *btnTextureCircleBkgd;
    QToolButton *btnRunTextureCompletion;
    QLabel lblTextureCircleBkgd;

    int iBrushSelected; // 1 -- small, 10 -- medium, 100 -- big

    // buttons for line toolbar
    QToolButton *btnLineDraw;
    QToolButton *btnLineClear;
    QToolButton *btnRunLineCompletion;

    // for dispaly and draw image
    TaoCanvasArea *canvasArea;
    QScrollArea *scrollArea;

    QString strQCurBasePath = "D://";
    QString inputFileName = "";
    QString strQFileAbsolutePath= "";
    QString strQFileCompleteBaseName;
    QString strQFileSuffix;

    QString savedFileName;

    TaoImageViewer viewerWindow;

    TaoProcessing *processing;



    bool blnSaved = true;
    int maxAllowedSaved = 5;
    int iCountSaved = 0;

    // sum margin to the top and bottom
    int displayMargin = 20;

    TaoHelpTemplate helpTmpl;
//    TaoRegister  *registerUser;

    TaoWxPay *wxPay;

    QByteArray saveRequest = "{\"guid\": \"1234567890\"}";
};

#endif // TAOMAIN_H



// style sheet
/*
QToolButton {
    margin: 3px;
}

QToolButton:checked, QToolButton:pressed {
    background-color: yellow;
    color: yellow;
}

QToolButton:hover {
    background-color: yellow;
    border: 1px solid;
}

QToolBar::separator {
    width: 3px;
}
*/
