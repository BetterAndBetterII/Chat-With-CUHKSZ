#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QSplitter>
#include <QListWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QMap>
#include <QList>
#include <QIcon>
#include <QKeyEvent>

class CustomTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit CustomTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {
        connect(this, &QTextEdit::textChanged, this, &CustomTextEdit::adjustHeight);
        setFontPointSize(12);  // 增加字体大小
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (event->modifiers() & Qt::ShiftModifier) {
                // 如果按下的是 Shift + Enter，插入换行
                insertPlainText("\n");
            } else {
                // 否则发送消息
                emit sendMessage(); // 触发自定义的信号
                event->accept();
            }
        } else {
            QTextEdit::keyPressEvent(event);
        }
    }

signals:
    void sendMessage(); // 自定义的信号，用于通知发送消息

private slots:
    void adjustHeight() {
        // 调整输入框的高度
        QFontMetrics fm(font());
        int lineHeight = fm.lineSpacing();
        int numLines = toPlainText().split('\n').count();  // 获取当前文本的行数
        int newHeight = (numLines + 1) * lineHeight + 10;  // 计算新的高度，加一些间隙

        // 设置高度上限，避免输入框无限增长
        newHeight = qBound(50, newHeight, 150);  // 最小高度 50，最大高度 150
        setFixedHeight(newHeight);
    }
};

class ChatMessageWidget : public QWidget {
    Q_OBJECT

public:
    ChatMessageWidget(const QString &text, const QString &avatarPath, bool isUser = true, QWidget *parent = nullptr)
        : QWidget(parent) {

        // 设置整体布局
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // 头像
        QLabel *avatar = new QLabel;
        avatar->setFixedSize(50, 50);
        avatar->setStyleSheet("border-radius: 25px;");
        avatar->setPixmap(QPixmap(avatarPath).scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

        // 气泡文本
        QLabel *message = new QLabel(text);
        message->setWordWrap(true);
        message->setStyleSheet(isUser
                               ? "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0078d7, stop:1 #005a9e); color: white; border-radius: 12px; padding: 12px;"
                               : "background-color: #f0f0f0; color: black; border-radius: 12px; padding: 12px;");
        message->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        message->setMaximumWidth(500);  // 增加气泡的最大宽度，使其能够容纳更多文字

        // 布局调整
        if (isUser) {
            mainLayout->addStretch();
            mainLayout->addWidget(message);
            mainLayout->addWidget(avatar);
        } else {
            mainLayout->addWidget(avatar);
            mainLayout->addWidget(message);
            mainLayout->addStretch();
        }
    }
};

class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // 创建主窗口布局
        auto *mainWidget = new QWidget(this);
        setCentralWidget(mainWidget);

        // 左侧导航栏：历史记录选择
        historyList = new QListWidget;
        historyList->setStyleSheet("QListWidget { background-color: #ffffff; border: none; padding: 10px; }"
                                   "QListWidget::item { padding: 10px; border-radius: 8px; }"
                                   "QListWidget::item:selected { background-color: #b0c4de; color: #000000; }");

        // 右侧聊天区域
        chatList = new QListWidget;
        chatList->setStyleSheet("background-color: #f9f9f9; border: none; padding: 10px;");

        // 消息输入区域
        messageInput = new CustomTextEdit;
        messageInput->setPlaceholderText("输入消息...");
        messageInput->setStyleSheet("background-color: #ffffff; border: 1px solid #ccc; border-radius: 12px; padding: 10px;");
        messageInput->setMinimumHeight(50);
        messageInput->setMaximumHeight(150);
        messageInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        auto *sendButton = new QPushButton("发送");
        sendButton->setStyleSheet("background-color: #0078d7; color: white; border-radius: 12px; padding: 10px 20px;");
        sendButton->setFixedHeight(50);

        // 输入区域布局
        auto *inputLayout = new QHBoxLayout;
        inputLayout->addWidget(messageInput);
        inputLayout->addWidget(sendButton);
        inputLayout->setSpacing(10);

        // 主聊天布局
        auto *chatLayout = new QVBoxLayout;
        chatLayout->addWidget(new QLabel("ChatGPT 模拟聊天窗口"));
        chatLayout->addWidget(chatList);
        chatLayout->addLayout(inputLayout);
        chatLayout->setSpacing(15);

        // 将右侧内容布局放入 QWidget 中
        auto *chatWidget = new QWidget;
        chatWidget->setLayout(chatLayout);

        // 使用 QSplitter 分割左侧导航栏和右侧聊天窗口
        auto *splitter = new QSplitter;
        splitter->addWidget(historyList);
        splitter->addWidget(chatWidget);
        splitter->setStretchFactor(1, 3);
        splitter->setHandleWidth(2);

        // 设置主布局
        auto *mainLayout = new QVBoxLayout(mainWidget);
        mainLayout->addWidget(splitter);

        // 会话历史记录保存
        currentSessionIndex = 0;  // 当前会话索引
        sessionHistory[currentSessionIndex] = QList<QString>();  // 新建一个空会话

        // 添加多个初始的历史记录
        addNewHistoryItem("会话 1");
        addNewHistoryItem("会话 2");
        addNewHistoryItem("会话 3");
        addNewHistoryItem("会话 4");
        addNewHistoryItem("会话 5");

        // 初始化每个会话的聊天记录（仅模拟）
        sessionHistory[1] = QList<QString> { "用户: 你好，今天天气怎么样？", "ChatGPT: 今天的天气很不错，阳光明媚！" };
        sessionHistory[2] = QList<QString> { "用户: 帮我查一下股票信息。", "ChatGPT: 目前无法直接查股票信息，但你可以访问相关网站。" };
        sessionHistory[3] = QList<QString> { "用户: 你能给我讲个笑话吗？", "ChatGPT: 当然啦，为什么程序员总是困惑？因为他们总在调试生活的 Bug！" };
        sessionHistory[4] = QList<QString> { "用户: 今天的新闻头条是什么？", "ChatGPT: 今天的头条是关于科技进展的新闻，新型量子计算机取得了重大突破。" };
        sessionHistory[5] = QList<QString> { "用户: 你会唱歌吗？", "ChatGPT: 我虽然不会唱歌，但我可以给你提供歌词！" };

        // 连接发送按钮功能
        connect(sendButton, &QPushButton::clicked, this, [this]() {
            sendMessage();
        });

        // 监听自定义输入框中的发送消息信号
        connect(messageInput, &CustomTextEdit::sendMessage, this, [this]() {
            sendMessage();
        });

        // 当点击历史记录时加载相应的会话
        connect(historyList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            int index = historyList->row(item);
            loadSession(index + 1);
        });

        setWindowTitle("ChatGPT 模拟界面");
        resize(1000, 700);

        // 设置样式表
        setStyleSheet(R"(
            QWidget {
                background-color: #f0f0f0;
                font-family: Arial, sans-serif;
                font-size: 14px;
            }
            QLabel {
                color: #555;
                font-size: 18px;
                padding: 10px 0;
            }
        )");
    }

    // 添加新历史记录项
    void addNewHistoryItem(const QString &title) {
        QListWidgetItem *item = new QListWidgetItem(title);
        historyList->addItem(item);
    }

    // 添加消息到聊天窗口
    void addMessage(QListWidget *chatList, const QString &message, const QString &avatarPath, bool isUser) {
        auto *item = new QListWidgetItem(chatList);
        auto *messageWidget = new ChatMessageWidget(message, avatarPath, isUser);
        item->setSizeHint(messageWidget->sizeHint());
        chatList->setItemWidget(item, messageWidget);
        chatList->scrollToBottom();
    }

    // 加载指定会话的所有消息
    void loadSession(int index) {
        if (sessionHistory.contains(index)) {
            chatList->clear();  // 清空当前聊天列表
            const QList<QString> &messages = sessionHistory[index];
            for (const QString &message : messages) {
                // 判断是用户还是ChatGPT消息
                bool isUser = message.startsWith("用户:");
                QString avatarPath = isUser ? ":../resources/picture/img.png" : ":../resources/picture/img_1.png";
                addMessage(chatList, message, avatarPath, isUser);
            }
            currentSessionIndex = index;  // 更新当前会话索引
        }
    }

    // 发送消息的方法
    void sendMessage() {
        QString message = messageInput->toPlainText().trimmed();
        if (!message.isEmpty()) {
            // 添加用户消息到当前会话
            addMessage(chatList, message, ":../resources/picture/img.png", true);
            sessionHistory[currentSessionIndex].append("用户: " + message);

            // 模拟 ChatGPT 回复
            QString response = "这是 ChatGPT 的回复: " + message;
            addMessage(chatList, response, ":../resources/picture/img_1.png", false);
            sessionHistory[currentSessionIndex].append("ChatGPT: " + response);

            messageInput->clear();
        }
    }

private:
    QListWidget *historyList;
    QListWidget *chatList;
    CustomTextEdit *messageInput;  // 将 messageInput 改为自定义的 CustomTextEdit，以便多行输入和处理 Enter 键
    QMap<int, QList<QString>> sessionHistory;  // 保存每个会话的消息
    int currentSessionIndex;  // 当前会话索引
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ChatWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
