/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef BASETEXTEDITOR_H
#define BASETEXTEDITOR_H

#include "basetextdocument.h"
#include "itexteditor.h"
#include "codeassist/assistenums.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/find/ifindsupport.h>

#include <QPlainTextEdit>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
class QToolBar;
class QPrinter;
QT_END_NAMESPACE

namespace Utils { class LineColumnLabel; }

namespace TextEditor {

class TabSettings;
class RefactorOverlay;
struct RefactorMarker;
class IAssistMonitorInterface;
class IAssistInterface;
class IAssistProvider;
class ICodeStylePreferences;
class CompletionAssistProvider;
typedef QList<RefactorMarker> RefactorMarkers;

namespace Internal {
    class BaseTextEditorWidgetPrivate;
    class TextEditorOverlay;
    typedef QString (QString::*TransformationMethod)() const;
}

class BaseTextEditorWidget;
class FontSettings;
class BehaviorSettings;
class CompletionSettings;
class DisplaySettings;
class MarginSettings;
class TypingSettings;
class StorageSettings;
class Indenter;
class AutoCompleter;
class ExtraEncodingSettings;

class TEXTEDITOR_EXPORT BaseTextEditor : public Core::IEditor
{
    Q_OBJECT

public:
    enum PositionOperation {
        Current = 1,
        EndOfLine = 2,
        StartOfLine = 3,
        Anchor = 4,
        EndOfDoc = 5
    };

    BaseTextEditor(BaseTextEditorWidget *editorWidget);
    ~BaseTextEditor();

    virtual BaseTextEditorDocument *textDocument();

    enum MarkRequestKind {
        BreakpointRequest,
        BookmarkRequest,
        TaskMarkRequest
    };

    static BaseTextEditor *currentTextEditor();

    friend class BaseTextEditorWidget;
    BaseTextEditorWidget *editorWidget() const { return m_editorWidget; }
    BaseTextDocument *baseTextDocument();

    // IEditor
    Core::IDocument *document();
    bool open(QString *errorString, const QString &fileName, const QString &realFileName);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);
    QWidget *toolBar();

    enum Side { Left, Right };
    void insertExtraToolBarWidget(Side side, QWidget *widget);

    QString contextHelpId() const; // from IContext

    int currentLine() const;
    int currentColumn() const;
    void gotoLine(int line, int column = 0, bool centerLine = true);

    /*! Returns the amount of visible columns (in characters) in the editor */
    int columnCount() const;

    /*! Returns the amount of visible lines (in characters) in the editor */
    int rowCount() const;

    /*! Returns the position at \a posOp in characters from the beginning of the document */
    virtual int position(PositionOperation posOp = Current, int at = -1) const;

    /*! Converts the \a pos in characters from beginning of document to \a line and \a column */
    virtual void convertPosition(int pos, int *line, int *column) const;

    /*! Returns the cursor rectangle in pixels at \a pos, or current position if \a pos = -1 */
    virtual QRect cursorRect(int pos = -1) const;

    virtual QString selectedText() const;

    /*! Removes \a length characters to the right of the cursor. */
    virtual void remove(int length);
    /*! Inserts the given string to the right of the cursor. */
    virtual void insert(const QString &string);
    /*! Replaces \a length characters to the right of the cursor with the given string. */
    virtual void replace(int length, const QString &string);
    /*! Sets current cursor position to \a pos. */
    virtual void setCursorPosition(int pos);
    /*! Selects text between current cursor position and \a toPos. */
    virtual void select(int toPos);

    virtual const Utils::CommentDefinition *commentDefinition() const;

    virtual CompletionAssistProvider *completionAssistProvider();

signals:
    void markRequested(TextEditor::BaseTextEditor *editor, int line, TextEditor::BaseTextEditor::MarkRequestKind kind);
    void markContextMenuRequested(TextEditor::BaseTextEditor *editor, int line, QMenu *menu);
    void tooltipOverrideRequested(TextEditor::BaseTextEditor *editor, const QPoint &globalPos, int position, bool *handled);
    void tooltipRequested(TextEditor::BaseTextEditor *editor, const QPoint &globalPos, int position);
    void markTooltipRequested(TextEditor::BaseTextEditor *editor, const QPoint &globalPos, int line);
    void contextHelpIdRequested(TextEditor::BaseTextEditor *editor, int position);

private slots:
    void updateCursorPosition();
    void openGotoLocator();
    void setFileEncodingLabelVisible(bool visible);
    void setFileEncodingLabelText(const QString &text);

private:
    // Note: This is always a copy of IContext::m_widget.
    BaseTextEditorWidget *m_editorWidget;

    QToolBar *m_toolBar;
    QWidget *m_stretchWidget;
    QAction *m_cursorPositionLabelAction;
    Utils::LineColumnLabel *m_cursorPositionLabel;
    QAction *m_fileEncodingLabelAction;
    Utils::LineColumnLabel *m_fileEncodingLabel;
};


class TEXTEDITOR_EXPORT BaseTextEditorWidget : public QPlainTextEdit
{
    Q_OBJECT
    Q_PROPERTY(int verticalBlockSelectionFirstColumn READ verticalBlockSelectionFirstColumn)
    Q_PROPERTY(int verticalBlockSelectionLastColumn READ verticalBlockSelectionLastColumn)

public:
    BaseTextEditorWidget(QWidget *parent = 0);
    BaseTextEditorWidget(BaseTextDocument *doc, QWidget *parent = 0);
    BaseTextEditorWidget(BaseTextEditorWidget *other);
    ~BaseTextEditorWidget();

    BaseTextDocument *baseTextDocument() const;

    // IEditor
    virtual bool open(QString *errorString, const QString &fileName, const QString &realFileName);
    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);
    void gotoLine(int line, int column = 0, bool centerLine = true);
    int position(BaseTextEditor::PositionOperation posOp = BaseTextEditor::Current,
         int at = -1) const;
    void convertPosition(int pos, int *line, int *column) const;

    BaseTextEditor *editor() const;

    void print(QPrinter *);

    void appendStandardContextMenuActions(QMenu *menu);

    // Works only in conjunction with a syntax highlighter that puts
    // parentheses into text block user data
    void setParenthesesMatchingEnabled(bool b);
    bool isParenthesesMatchingEnabled() const;

    void setHighlightCurrentLine(bool b);
    bool highlightCurrentLine() const;

    void setLineNumbersVisible(bool b);
    bool lineNumbersVisible() const;

    void setAlwaysOpenLinksInNextSplit(bool b);
    bool alwaysOpenLinksInNextSplit() const;

    void setMarksVisible(bool b);
    bool marksVisible() const;

    void setRequestMarkEnabled(bool b);
    bool requestMarkEnabled() const;

    void setLineSeparatorsAllowed(bool b);
    bool lineSeparatorsAllowed() const;

    void updateCodeFoldingVisible();
    bool codeFoldingVisible() const;

    void setCodeFoldingSupported(bool b);
    bool codeFoldingSupported() const;

    void setMouseNavigationEnabled(bool b);
    bool mouseNavigationEnabled() const;

    void setMouseHidingEnabled(bool b);
    bool mouseHidingEnabled() const;

    void setScrollWheelZoomingEnabled(bool b);
    bool scrollWheelZoomingEnabled() const;

    void setConstrainTooltips(bool b);
    bool constrainTooltips() const;

    void setCamelCaseNavigationEnabled(bool b);
    bool camelCaseNavigationEnabled() const;

    void setRevisionsVisible(bool b);
    bool revisionsVisible() const;

    void setVisibleWrapColumn(int column);
    int visibleWrapColumn() const;

    int columnCount() const;
    int rowCount() const;

    void setReadOnly(bool b);

    void setTextCursor(const QTextCursor &cursor, bool keepBlockSelection);
    void setTextCursor(const QTextCursor &cursor);

    void insertCodeSnippet(const QTextCursor &cursor, const QString &snippet);

    void setBlockSelection(bool on);
    void setBlockSelection(int positionBlock, int positionColumn, int anchhorBlock,
                           int anchorColumn);
    void setBlockSelection(const QTextCursor &cursor);
    QTextCursor blockSelection() const;
    bool hasBlockSelection() const;

    int verticalBlockSelectionFirstColumn() const;
    int verticalBlockSelectionLastColumn() const;

    QRegion translatedLineRegion(int lineStart, int lineEnd) const;

    void setAutoCompleter(AutoCompleter *autoCompleter);
    AutoCompleter *autoCompleter() const;

    QPoint toolTipPosition(const QTextCursor &c) const;

    void invokeAssist(AssistKind assistKind, IAssistProvider *provider = 0);

    virtual IAssistInterface *createAssistInterface(AssistKind assistKind,
                                                    AssistReason assistReason) const;
    static QMimeData *duplicateMimeData(const QMimeData *source);

    static QString msgTextTooLarge(quint64 size);

    void insertPlainText(const QString &text);


public slots:
    virtual void copy();
    virtual void paste();
    virtual void cut();
    virtual void selectAll();

    void circularPaste();
    void switchUtf8bom();

    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    void zoomReset();

    void cutLine();
    void copyLine();
    void deleteLine();
    void deleteEndOfWord();
    void deleteEndOfWordCamelCase();
    void deleteStartOfWord();
    void deleteStartOfWordCamelCase();
    void unfoldAll();
    void fold();
    void unfold();
    void selectEncoding();
    void updateTextCodecLabel();

    void gotoBlockStart();
    void gotoBlockEnd();
    void gotoBlockStartWithSelection();
    void gotoBlockEndWithSelection();

    void gotoLineStart();
    void gotoLineStartWithSelection();
    void gotoLineEnd();
    void gotoLineEndWithSelection();
    void gotoNextLine();
    void gotoNextLineWithSelection();
    void gotoPreviousLine();
    void gotoPreviousLineWithSelection();
    void gotoPreviousCharacter();
    void gotoPreviousCharacterWithSelection();
    void gotoNextCharacter();
    void gotoNextCharacterWithSelection();
    void gotoPreviousWord();
    void gotoPreviousWordWithSelection();
    void gotoNextWord();
    void gotoNextWordWithSelection();
    void gotoPreviousWordCamelCase();
    void gotoPreviousWordCamelCaseWithSelection();
    void gotoNextWordCamelCase();
    void gotoNextWordCamelCaseWithSelection();

    bool selectBlockUp();
    bool selectBlockDown();

    void moveLineUp();
    void moveLineDown();

    void viewPageUp();
    void viewPageDown();
    void viewLineUp();
    void viewLineDown();

    void copyLineUp();
    void copyLineDown();

    void joinLines();

    void insertLineAbove();
    void insertLineBelow();

    void uppercaseSelection();
    void lowercaseSelection();

    void cleanWhitespace();

    void indent();
    void unindent();

    void undo();
    void redo();

    void openLinkUnderCursor();
    void openLinkUnderCursorInNextSplit();

    /// Abort code assistant if it is running.
    void abortAssist();

signals:
    void assistFinished();
    void readOnlyChanged();

protected:
    bool event(QEvent *e);
    void inputMethodEvent(QInputMethodEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void changeEvent(QEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    void showEvent(QShowEvent *);

    QMimeData *createMimeDataFromSelection() const;
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);

    virtual QString plainTextFromSelection(const QTextCursor &cursor) const;
    static QString convertToPlainText(const QString &txt);

    virtual QString lineNumber(int blockNumber) const;
    virtual int lineNumberDigits() const;
    virtual bool selectionVisible(int blockNumber) const;
    virtual bool replacementVisible(int blockNumber) const;
    virtual QColor replacementPenColor(int blockNumber) const;

private:
    void maybeSelectLine();
    void updateCannotDecodeInfo();
    void collectToCircularClipboard();

protected:
    virtual BaseTextEditor *createEditor() = 0;
    virtual void triggerPendingUpdates();
    virtual void applyFontSettings();

private slots:
    void editorContentsChange(int position, int charsRemoved, int charsAdded);
    void documentAboutToBeReloaded();
    void documentReloadFinished(bool success);
    void highlightSearchResults(const QString &txt, Core::FindFlags findFlags);
    void setFindScope(const QTextCursor &start, const QTextCursor &end, int, int);
    bool inFindScope(const QTextCursor &cursor);
    bool inFindScope(int selectionStart, int selectionEnd);
    void inSnippetMode(bool *active);

private:
    Internal::BaseTextEditorWidgetPrivate *d;
    friend class Internal::BaseTextEditorWidgetPrivate;
    friend class Internal::TextEditorOverlay;
    friend class RefactorOverlay;

public:
    QWidget *extraArea() const;
    virtual int extraAreaWidth(int *markWidthPtr = 0) const;
    virtual void extraAreaPaintEvent(QPaintEvent *);
    virtual void extraAreaLeaveEvent(QEvent *);
    virtual void extraAreaContextMenuEvent(QContextMenuEvent *);
    virtual void extraAreaMouseEvent(QMouseEvent *);
    void updateFoldingHighlight(const QPoint &pos);

    void setLanguageSettingsId(Core::Id settingsId);
    Core::Id languageSettingsId() const;

    void setCodeStyle(ICodeStylePreferences *settings);

    const DisplaySettings &displaySettings() const;
    const MarginSettings &marginSettings() const;

    void ensureCursorVisible();

    enum ExtraSelectionKind {
        CurrentLineSelection,
        ParenthesesMatchingSelection,
        CodeWarningsSelection,
        CodeSemanticsSelection,
        UndefinedSymbolSelection,
        UnusedSymbolSelection,
        FakeVimSelection,
        OtherSelection,
        SnippetPlaceholderSelection,
        ObjCSelection,
        DebuggerExceptionSelection,
        NExtraSelectionKinds
    };
    void setExtraSelections(ExtraSelectionKind kind, const QList<QTextEdit::ExtraSelection> &selections);
    QList<QTextEdit::ExtraSelection> extraSelections(ExtraSelectionKind kind) const;
    QString extraSelectionTooltip(int pos) const;

    RefactorMarkers refactorMarkers() const;
    void setRefactorMarkers(const RefactorMarkers &markers);
signals:
    void refactorMarkerClicked(const TextEditor::RefactorMarker &marker);

public:
    // the blocks list must be sorted
    void setIfdefedOutBlocks(const QList<BlockRange> &blocks);

public slots:
    virtual void format();
    virtual void rewrapParagraph();
    virtual void unCommentSelection();
    virtual void setDisplaySettings(const TextEditor::DisplaySettings &);
    virtual void setMarginSettings(const TextEditor::MarginSettings &);
    virtual void setBehaviorSettings(const TextEditor::BehaviorSettings &);
    virtual void setTypingSettings(const TextEditor::TypingSettings &);
    virtual void setStorageSettings(const TextEditor::StorageSettings &);
    virtual void setCompletionSettings(const TextEditor::CompletionSettings &);
    virtual void setExtraEncodingSettings(const TextEditor::ExtraEncodingSettings &);

protected:
    bool viewportEvent(QEvent *event);

    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void leaveEvent(QEvent *);
    void keyReleaseEvent(QKeyEvent *);

    void dragEnterEvent(QDragEnterEvent *e);

    void showDefaultContextMenu(QContextMenuEvent *e, Core::Id menuContextId);

public:
    struct Link
    {
        Link(const QString &fileName = QString(), int line = 0, int column = 0)
            : linkTextStart(-1)
            , linkTextEnd(-1)
            , targetFileName(fileName)
            , targetLine(line)
            , targetColumn(column)
        {}

        bool hasValidTarget() const
        { return !targetFileName.isEmpty(); }

        bool hasValidLinkText() const
        { return linkTextStart != linkTextEnd; }

        bool operator==(const Link &other) const
        { return linkTextStart == other.linkTextStart && linkTextEnd == other.linkTextEnd; }

        int linkTextStart;
        int linkTextEnd;

        QString targetFileName;
        int targetLine;
        int targetColumn;
    };

    QString selectedText() const;

protected:
    /*!
       Reimplement this function to enable code navigation.

       \a resolveTarget is set to true when the target of the link is relevant
       (it isn't until the link is used).
     */
    virtual Link findLinkAt(const QTextCursor &, bool resolveTarget = true,
                            bool inNextSplit = false);

    /*!
       Reimplement this function if you want to customize the way a link is
       opened. Returns whether the link was opened successfully.
     */
    virtual bool openLink(const Link &link, bool inNextSplit = false);

    /*!
      Reimplement this function to change the default replacement text.
      */
    virtual QString foldReplacementText(const QTextBlock &block) const;
    virtual void drawCollapsedBlockPopup(QPainter &painter,
                                         const QTextBlock &block,
                                         QPointF offset,
                                         const QRect &clip);
    int visibleFoldedBlockNumber() const;


protected slots:
    virtual void slotUpdateExtraArea();
    virtual void slotUpdateExtraAreaWidth();
    virtual void slotUpdateRequest(const QRect &r, int dy);
    virtual void slotCursorPositionChanged();
    virtual void slotUpdateBlockNotify(const QTextBlock &);
    virtual void slotCodeStyleSettingsChanged(const QVariant &);

signals:
    void requestFontZoom(int zoom);
    void requestZoomReset();
    void requestBlockUpdate(const QTextBlock &);

private:
    void ctor(const QSharedPointer<BaseTextDocument> &doc);
    void handleHomeKey(bool anchor);
    void handleBackspaceKey();
    void moveLineUpDown(bool up);
    void copyLineUpDown(bool up);
    void saveCurrentCursorPositionForNavigation();
    void updateHighlights();
    void updateCurrentLineHighlight();

    void drawFoldingMarker(QPainter *painter, const QPalette &pal,
                           const QRect &rect,
                           bool expanded,
                           bool active,
                           bool hovered) const;

    void toggleBlockVisible(const QTextBlock &block);
    QRect foldBox();

    QTextBlock foldedBlockAt(const QPoint &pos, QRect *box = 0) const;

    void updateLink(QMouseEvent *e);
    void showLink(const Link &);
    void clearLink();

    void universalHelper(); // test function for development

    bool cursorMoveKeyEvent(QKeyEvent *e);
    bool camelCaseRight(QTextCursor &cursor, QTextCursor::MoveMode mode);
    bool camelCaseLeft(QTextCursor &cursor, QTextCursor::MoveMode mode);

    void processTooltipRequest(const QTextCursor &c);

    void transformSelection(Internal::TransformationMethod method);
    void transformBlockSelection(Internal::TransformationMethod method);

private slots:
    void updateTabStops();
    void applyFontSettingsDelayed();

    // parentheses matcher
    void _q_matchParentheses();
    void _q_highlightBlocks();
    void slotSelectionChanged();
    void _q_animateUpdate(int position, QPointF lastPos, QRectF rect);
    void doFoo();
};

} // namespace TextEditor

Q_DECLARE_METATYPE(TextEditor::BaseTextEditorWidget::Link)

#endif // BASETEXTEDITOR_H
