#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "noteview.h"
#include "panel.h"

#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <QWidget>

class KActionCollection;

class QUrl;
class QVariant;
class QApplication;
class QListView;
class QSplitter;
class QModelIndex;

class MainView : public Panel 
{
    Q_OBJECT
    
public:
    Markpado *markPad;
    KTextEditor::Document *note;
    NoteView *noteView;
    
    MainView(QWidget *parent = 0, KActionCollection *actions = 0);
    virtual ~MainView();    
    KTextEditor::View *getEditor();
    bool preview();
    bool unpreview();
    void openUrl(QUrl url);
    void setupUI();
    
private:
    QVBoxLayout *verticalLayout;
    QSplitter *vsplitter;
    QSplitter *hsplitter;
    Panel *terminal;
    Panel *navigator;
    Panel *listPanel;
    KActionCollection *actions;
    
    KTextEditor::View *editor;
    int column;

public slots:
    void newNote();
    void goHome();
    
private slots:
    void oneColView();
    void twoColView();
    void threeColView();
    void toggleTerminal();
    void slotOpen(const QUrl &url);
    
protected:
    virtual bool urlChanged();
};

#endif // MAINVIEW_H