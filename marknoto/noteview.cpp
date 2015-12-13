#include "noteview.h"
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KActionCollection>
#include <KFileMetaData/UserMetaData>

#include <QAction>
#include <QDir>
#include <QVBoxLayout>
#include <QUrl>
#include <QDesktopServices>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>

NoteView::NoteView(QWidget* parent, KActionCollection *pActions)
    : QWidget(parent),
    actions(pActions)
{
    done = new QStack<QUrl>();
    todo = new QStack<QUrl>();
    setupUI();
    setupConnect();
}

void NoteView::pureOpenUrl(const QUrl& url)
{
    // TODO:if the url is not in the watching dir and is in three column view
    // switch to one column view
    title->setText(url.fileName());
    note->openUrl(url);
    if (actions->action("file_preview")->isChecked())
        markPad->preview();
    else
        markPad->unpreview();
    
    KFileMetaData::UserMetaData metaData(url.toLocalFile());
    const QStringList list = metaData.tags();
    for (int i=0; i<list.size(); i++)
        tagList->addItem(list.at(i));
}

void NoteView::setupConnect()
{
    connect(title, SIGNAL(returnPressed(QString)),
        this, SLOT(saveNote(QString)));
}

void NoteView::setupUI()
{
    vl = new QVBoxLayout(this);
    hl = new QHBoxLayout(this);
    title = new QLineEdit(this);
    tagList = new QListWidget(this);
    label = new QLabel(this);
    hl->addWidget(label);
    hl->addWidget(tagList);
    
    markPad = new Markpado(this);
    note = markPad->m_note;
    
    vl->addWidget(title);
    vl->addWidget(markPad);
    vl->addLayout(hl);
    
    title->setFixedHeight(24);
    title->setContentsMargins(0, 0, 0, 0);
    title->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("QLineEdit { border: 1px solid lightskyblue; border-radius: 2px; }");
    
    tagList->setContentsMargins(0, 0, 0, 0);
    tagList->setFixedHeight(24);
    tagList->sizePolicy().setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    tagList->setFrameShape(QFrame::NoFrame);
    
    label->setPixmap(QIcon::fromTheme(QLatin1String("tag")).pixmap(24, 24));
    
    hl->setSpacing(0);
    vl->setSpacing(0);
    
    tagList->setFlow(QListView::LeftToRight);
}

void NoteView::saveNote(QString name)
{
    QUrl url = note->url();
    
    note->documentSave();
    if (QFileInfo(url.path()).isDir()) {
        url = QUrl::fromLocalFile(url.path().append("/").append(name));
    } else
        url.setUrl(url.url(QUrl::RemoveFilename).append(name));
    
    QDir dir(url.url(QUrl::RemoveFilename | QUrl::PreferLocalFile));
    dir.rename(note->url().fileName(), url.fileName());
    note->openUrl(url);
    markPad->view()->setFocus();
}

void NoteView::hideTitleLine()
{
    title->setHidden(true);
}

void NoteView::showTitleLine()
{
    title->setHidden(false);
}

void NoteView::setTitle(const QString& titleOfNote)
{
    title->setText(titleOfNote);
}

void NoteView::forward()
{
    QUrl url(todo->pop());
    done->push(url);
    pureOpenUrl(url);
}

void NoteView::backward()
{
    QUrl url(done->pop());
    todo->push(url);
    pureOpenUrl(done->top());
}

bool NoteView::canBackward()
{
    if (done->count() > 1)
        return true;
    else
        return false;
}

bool NoteView::canForward()
{
    if (todo->isEmpty())
        return false;
    else
        return true;
}

void NoteView::openUrl(const QUrl& url)
{
    pureOpenUrl(url);
    if (done->isEmpty() || url != done->top())
        done->push(url);
    if (canBackward())
        actions->action("go_backward")->setChecked(false);
    delete todo;
    todo = new QStack<QUrl>();
    actions->action("go_forward")->setChecked(true);
    
    /*
    QVectorIterator<QUrl> i(*done);
    while (i.hasNext())
        qDebug() << i.next();
    */
}

// Open url that from the note(links)
void NoteView::slotOpen(const QUrl& url)
{
    if (url.toString().startsWith("file:///")) {
        // case1: fake root link
        KConfigGroup cfg(KSharedConfig::openConfig(), "General Options");
        QString rootPath(cfg.readEntry("NoteDir"));
        QString notePath(url.toString().replace("file:///", "/"));
        QUrl newUrl("file://" + rootPath + notePath);
        qDebug() << newUrl;
        if (QFileInfo(newUrl.path()).exists()) {
            openUrl(newUrl);
            return ;
        }

        // case2: relative path link(automatically handled by API)
        openUrl(url);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void NoteView::focusTitle()
{
    title->setFocus();
}

NoteView::~NoteView()
{
    delete tagWidget;
    delete done;
    delete todo;
}

#include "noteview.moc"
