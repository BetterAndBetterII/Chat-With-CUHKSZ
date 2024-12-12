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
#include <QPixmap>
#include <qpainter.h>
#include <QFrame>
#include <QScroller>
#include <QStackedLayout>
#include <QMap>
#include <QList>
#include <QIcon>
#include <QKeyEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QGridLayout>
#include <QWidget>
#include <QStackedWidget>
#include <QApplication>
#include <QMainWindow>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QListView>
#include "../include/Client/Client.h" // 根据 include 的实际路径
// #include "../include/third_party/httplib.h"
#include <iostream>
#include <QApplication>
#include "qnchatmessage.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMessageBox>
#include <QUuid>
#include <QTimer>
#include <QRegularExpression>

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
        newHeight = qBound(80, newHeight, 80);  // 最小高度 50，最大高度 150
        setFixedHeight(120);
    }
};

class WelcomeWindow : public QWidget {
    Q_OBJECT

public:
    WelcomeWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Settings");
        setFixedSize(1200, 900);
        setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);  // 设置窗口不可调整大小

        QLinearGradient gradient(0, 0, 1, 1);
        gradient.setColorAt(0, QColor(48, 48, 55)); // 背景渐变色，浅色
        gradient.setColorAt(1, QColor(28, 28, 36)); // 背景渐变色，深色
        QPalette palette;
        palette.setBrush(QPalette::Window, gradient);
        setPalette(palette);

        // 主布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // 顶部横幅布局
        QHBoxLayout *topLayout = new QHBoxLayout();
        QLabel *titleLabel = new QLabel("Chat With CUHKSZ", this);
        titleLabel->setStyleSheet("font-size: 40px; "
            "font-family: Arial, sans-serif;"
            "font-weight: bold; "
            "color: #444444; "
            "text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);");
        topLayout->addStretch(); // 添加左侧弹性空间
        topLayout->addWidget(titleLabel);
        topLayout->addStretch(); // 添加右侧弹性空间

        // 修改中部网格布局的设置
        QVBoxLayout *gridLayout = new QVBoxLayout();
        gridLayout->addSpacing(30);
        // 移除原有的边距设置，使用新的方式来居中
        // gridLayout->setContentsMargins(400, 100, 400, 100);

        // 按钮1: bb
        QPushButton *buttonBB = new QPushButton("BB System", this);
        buttonBB->setFixedSize(500, 40);
        buttonBB->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonBB, 0, 0);
        // 为每个按钮创建水平布局来实现居中
        QHBoxLayout* buttonLayoutBB = new QHBoxLayout();
        buttonLayoutBB->addStretch();  // 左侧弹性空间
        buttonLayoutBB->addWidget(buttonBB);
        buttonLayoutBB->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutBB);

        // 按钮2: sis
        QPushButton *buttonSIS = new QPushButton("SIS", this);
        buttonSIS->setFixedSize(500, 35);
        buttonSIS->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonSIS, 0, 1);
        QHBoxLayout* buttonLayoutSIS = new QHBoxLayout();
        buttonLayoutSIS->addStretch();  // 左侧弹性空间
        buttonLayoutSIS->addWidget(buttonSIS);
        buttonLayoutSIS->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutSIS);

        // 按钮3: Chat with CUHKSZ
        QPushButton *buttonChat = new QPushButton("Back to Chat!", this);
        buttonChat->setFixedSize(500, 35);
        buttonChat->setStyleSheet("font-size: 18px; font-weight: bold; background-color: #006699; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonChat, 0, 2, 1, 2); // 跨两列
        QHBoxLayout* buttonLayoutChat = new QHBoxLayout();
        buttonLayoutChat->addStretch();  // 左侧弹性空间
        buttonLayoutChat->addWidget(buttonChat);
        buttonLayoutChat->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutChat);

        // 按钮4: Booking
        QPushButton *buttonBooking = new QPushButton("Booking", this);
        buttonBooking->setFixedSize(500, 35);
        buttonBooking->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonBooking, 1, 0, 1, 2); // 跨两列
        QHBoxLayout* buttonLayoutBooking = new QHBoxLayout();
        buttonLayoutBooking->addStretch();  // 左侧弹性空间
        buttonLayoutBooking->addWidget(buttonBooking);
        buttonLayoutBooking->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutBooking);

        // 按钮5: Off
        QPushButton *buttonOfficial = new QPushButton("Official", this);
        buttonOfficial->setFixedSize(500, 35);
        buttonOfficial->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(buttonOfficial, 1, 2);
        QHBoxLayout* buttonLayoutOfficial = new QHBoxLayout();
        buttonLayoutOfficial->addStretch();  // 左侧弹性空间
        buttonLayoutOfficial->addWidget(buttonOfficial);
        buttonLayoutOfficial->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutOfficial);

        // 按钮6: 语言切换按钮
        QPushButton *buttonLanguage = new QPushButton("Language: ENGLISH", this);
        buttonLanguage->setFixedSize(500, 35);
        buttonLanguage->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonLanguage, 1, 3);
        QHBoxLayout* buttonLayoutLanguage = new QHBoxLayout();
        buttonLayoutLanguage->addStretch();  // 左侧弹性空间
        buttonLayoutLanguage->addWidget(buttonLanguage);
        buttonLayoutLanguage->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutLanguage);

        // 按钮7: vpn
        QPushButton *buttonVpn = new QPushButton("VPN", this);
        buttonVpn->setFixedSize(500, 35);
        buttonVpn->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(buttonVpn, 2, 2);
        QHBoxLayout* buttonLayoutVpn = new QHBoxLayout();
        buttonLayoutVpn->addStretch();  // 左侧弹性空间
        buttonLayoutVpn->addWidget(buttonVpn);
        buttonLayoutVpn->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayoutVpn);

        // 按钮8: 空按钮
        QPushButton *button4 = new QPushButton("Others", this);
        button4->setFixedSize(500, 35);
        button4->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(button4, 2, 3);
        QHBoxLayout* buttonLayout4 = new QHBoxLayout();
        buttonLayout4->addStretch();  // 左侧弹性空间
        buttonLayout4->addWidget(button4);
        buttonLayout4->addStretch();  // 右侧弹性空间
        gridLayout->addLayout(buttonLayout4);

        // 调整主布局
        mainLayout->addLayout(topLayout);  // 顶部标题
        mainLayout->addStretch();         // 上方弹性空间
        mainLayout->addLayout(gridLayout); // 按钮区域
        mainLayout->addStretch();         // 下方弹性空间

        // 设置整体布局的边距
        mainLayout->setContentsMargins(50, 30, 50, 30);  // 左、上、右、下的边距

        // 在 Booking 下方添加两行文字，紧贴 Booking 按钮
        QHBoxLayout *textAndButtonLayout = new QHBoxLayout();
        QLabel *bookingDescription1 = new QLabel("Combining Tradition with Modernity", this);
        QLabel *bookingDescription2 = new QLabel("Bridging China and the West", this);
        bookingDescription1->setAlignment(Qt::AlignCenter);
        bookingDescription2->setAlignment(Qt::AlignCenter);
        bookingDescription1->setStyleSheet("font-size: 10px; font-family: Arial; font-style: italic; color: black;");
        bookingDescription2->setStyleSheet("font-size: 10px; font-family: Arial; font-style: italic; color: black;");

        textAndButtonLayout->addWidget(bookingDescription1);
        textAndButtonLayout->addStretch();
        textAndButtonLayout->addWidget(bookingDescription2);

        // 现在添加到主布局
        mainLayout->addLayout(textAndButtonLayout);

        // 底部栏布局
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        bottomLayout->addStretch(); // 用于底部空白区域占位
        mainLayout->addLayout(bottomLayout);

        // 设置主布局
        setLayout(mainLayout);

        // 信号连接：点击 "Chat with CUHKSZ" 按钮时触发 startChat 信号
        connect(buttonChat, &QPushButton::clicked, this, &WelcomeWindow::goToChatWindow);

        // 信号连接：点击 "大学官网" 按钮时打开链接
        connect(buttonOfficial, &QPushButton::clicked, this, []() {
            QDesktopServices::openUrl(QUrl("https://www.cuhk.edu.cn"));
        });

        // 信号连接：bb 按钮跳转
        connect(buttonBB, &QPushButton::clicked, this, []() {
            QDesktopServices::openUrl(QUrl("https://bb.cuhk.edu.cn"));
        });

        // 信号连接：sis 按钮跳转
        connect(buttonSIS, &QPushButton::clicked, this, []() {
            QDesktopServices::openUrl(QUrl("https://sis.cuhk.edu.cn"));
        });

        // 信号连接：Booking 按钮跳转
        connect(buttonBooking, &QPushButton::clicked, this, []() {
            QDesktopServices::openUrl(QUrl("https://booking.cuhk.edu.cn"));
        });

        connect(buttonVpn, &QPushButton::clicked, this, []() {
            QDesktopServices::openUrl(QUrl("https://vpn.cuhk.edu.cn"));
        });

        // 信号连接：切换语言按钮功能
        connect(buttonLanguage, &QPushButton::clicked, this, [=]() {
            if (buttonLanguage->text() == "Language: ENGLISH") {
                // 切换到中文
                buttonLanguage->setText("语言：中文");

                // 更新其他按钮的文本
                buttonBB->setText("在线教学平台");
                buttonSIS->setText("学生信息系统");
                buttonBooking->setText("在线预定平台");
                buttonOfficial->setText("大学官网");

                bookingDescription1->setText("结合传统与现代");
                bookingDescription2->setText("融汇中国与西方");
                bookingDescription1->setStyleSheet("font-size: 24px; "
                    "font-family: 'Noto Sans CJK SC', 'WenQuanYi Micro Hei', sans-serif; "
                    "font-style: italic; "
                    "color: black;");
                bookingDescription2->setStyleSheet("font-size: 24px; "
                    "font-family: 'Noto Sans CJK SC', 'WenQuanYi Micro Hei', sans-serif; "
                    "font-style: italic; "
                    "color: black;");
            } else {
                // 切换回英文
                buttonLanguage->setText("Language: ENGLISH");

                // 恢复其他按钮的文本
                buttonBB->setText("BB System");
                buttonSIS->setText("SIS");
                buttonBooking->setText("Booking");
                buttonOfficial->setText("Official");

                bookingDescription1->setText("Combining Tradition with Modernity");
                bookingDescription2->setText("Bridging China and the West");
                bookingDescription1->setStyleSheet("font-size: 10px; font-family: Arial; font-style: italic; color: black;");
                bookingDescription2->setStyleSheet("font-size: 10px; font-family: Arial; font-style: italic; color: black;");
            }
        });
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        
        // 加载原始图片
        QPixmap originalPixmap(":/img/welcome2.png");
        
        // 获取窗口大小
        QSize windowSize = size();
        
        // 将图片缩放到窗口大小，保持纵横比
        QPixmap scaledPixmap = originalPixmap.scaled(windowSize,
                                                    Qt::KeepAspectRatioByExpanding,  // 改用KeepAspectRatioByExpanding
                                                    Qt::SmoothTransformation);
        
        // 如果缩放后的图片大于窗口，裁剪掉多余部分
        if (scaledPixmap.width() > width() || scaledPixmap.height() > height()) {
            int x = (scaledPixmap.width() - width()) / 2;
            int y = (scaledPixmap.height() - height()) / 2;
            scaledPixmap = scaledPixmap.copy(x, y, width(), height());
        }
        
        // 绘制图片
        painter.drawPixmap(0, 0, scaledPixmap);
    }
signals:
    void goToChatWindow();; // 自定义信号，用于通知主程序切换到聊天窗口
};

class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    ChatWindow(Client *client, QWidget *parent = nullptr)
      : QMainWindow(parent), client(client) {    // 创建主窗口布局

        auto *mainWidget = new QWidget(this);
        setCentralWidget(mainWidget);
        setWindowTitle("ChatWindow");
        setFixedSize(1200, 900);
        setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);  // 设置窗口不可调整大小
        // 左侧导航栏：历史记录选择
        historyList = new QListWidget;
        // historyList->setStyleSheet("QListWidget { background-color: #ffffff; border: none; padding: 10px; }"
        //                            "QListWidget::item { padding: 10px; border-radius: 8px; }"
        //                            "QListWidget::item:selected { background-color: #b0c4de; color: #000000; }");
        historyList->setStyleSheet(
            "QListWidget::item {background-color: #ffffff;color: #000000; border: transparent;border-bottom: 1px solid #dbdbdb;padding: 8px;}"
            "QListWidget::item:hover {background-color: #f5f5f5;}"
            "QListWidget::item:selected {border-left: 5px solid #777777;}");
        //设置为列表显示模式
        historyList->setViewMode(QListView::ListMode);
        //屏蔽水平滑动条
        historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // 右侧聊天区域
        chatList = new QListWidget;
        chatList->setFixedSize(850, 650);
        chatList->setStyleSheet("background-color: #f9f9f9; border: none; padding: 10px;");
        // 添加欢迎消息标签
        welcomeLabel = new QLabel("What can I help you with?", chatList);
        welcomeLabel->setAlignment(Qt::AlignCenter);
        welcomeLabel->setStyleSheet(
            "QLabel {"
            "   font-size: 24px;"
            "   color: #666666;"
            "   font-weight: bold;"
            "   background: transparent;"
            "}"
        );
        welcomeLabel->setFixedSize(850, 650);
        welcomeLabel->show();  // 初始时隐藏
        //设置为列表显示模式
        chatList->setViewMode(QListView::ListMode);
        //屏蔽水平滑动条
        chatList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        chatList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //设置为像素滚动
        chatList->setHorizontalScrollMode(QListWidget::ScrollPerPixel);
        //设置鼠标左键拖动
        // QScroller::grabGesture(chatList, QScroller::LeftMouseButtonGesture);

        // 消息输入区域
        messageInput = new CustomTextEdit;
        messageInput->setPlaceholderText("Text anything！");
        messageInput->setStyleSheet("background-color: #ffffff; border: 1px solid #ccc; border-radius: 12px; padding: 10px;");
        messageInput->setMinimumHeight(100);
        messageInput->setMaximumHeight(100);
        messageInput->setFixedSize(850, 150);
        messageInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QHBoxLayout *hbLayout = new QHBoxLayout;
        QVBoxLayout *vbLayout = new QVBoxLayout;
        sendButton = new QPushButton("Send");
        sendButton->setStyleSheet("background-color: #0078d7; color: white; border-radius: 12px; padding: 10px 20px;");
        sendButton->setFixedSize(100, 40);
        hbLayout->addStretch();
        hbLayout->addWidget(sendButton);
        // hbLayout->setMargin(2);
        vbLayout->addStretch();
        vbLayout->addLayout(hbLayout);
        // vbLayout->setMargin(2);
        // 输入区域布局
        messageInput->setLayout(vbLayout);

        QPushButton *backButton = new QPushButton("Settings", this);
        backButton->setStyleSheet("background-color: #0078d7; color: white; font-size: 18px; border-radius: 10px;");
        backButton->setFixedSize(120,40);
        connect(backButton, &QPushButton::clicked, this, &ChatWindow::backToWelcomeWindow);

        // 主聊天布局
        auto *chatLayout = new QVBoxLayout;
        auto *titleHBoxLayout = new QHBoxLayout;
        QPushButton *newChatButton = new QPushButton("New Chat", this);
        newChatButton->setStyleSheet("background-color: #0078d7; color: white; font-size: 18px; border-radius: 10px;");
        newChatButton->setFixedSize(120,40);

        titleHBoxLayout->addWidget(newChatButton);
        titleHBoxLayout->addWidget(new QLabel("AI Agent Based on ChatGPT-4o"));
        titleHBoxLayout->addStretch();
        titleHBoxLayout->addWidget(backButton);
        chatLayout->addLayout(titleHBoxLayout);
        chatLayout->addWidget(chatList);
        chatLayout->addWidget(messageInput);
        chatLayout->setSpacing(15);
        chatLayout->setMargin(5);

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
        // mainLayout->addWidget(backButton);
        // mainLayout->setAlignment(backButton, Qt::AlignLeft);

        // 获取历史记录第一行并更新列表
        updateHistoryList();

        // 会话历史记录保存
        currentSessionIndex = 0;  // 当前会话索引

        // 连接发送按钮功能
        connect(sendButton, &QPushButton::clicked, this, [this]() {
            sendMessage();
        });
        connect(messageInput, &CustomTextEdit::sendMessage, this, &ChatWindow::sendMessage);
        // 监听自定义输入框中的发送消息信号
        connect(messageInput, &CustomTextEdit::sendMessage, this, [this]() {
            sendMessage();
        });

        // 当点击历史记录时加载相应的会话
        connect(historyList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            int index = historyList->row(item);
            loadChatHistory(index);
        });

        // 添加 New Chat 按钮的功能
        connect(newChatButton, &QPushButton::clicked, this, [this]() {
            chatList->clear();
            messageInput->clear();
            updateWelcomeLabel();  // 添加这一行
            session_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
            // 创建新的会话
            currentSessionIndex = historyList->count();
        });

        setWindowTitle("Chat_With_CUHKSZ >_<");

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

        // 添加信号连接
        connect(this, &ChatWindow::messageReceived, 
                this, &ChatWindow::handleReceivedMessage, 
                Qt::QueuedConnection);
        
        connect(this, &ChatWindow::errorOccurred,
                this, &ChatWindow::handleError,
                Qt::QueuedConnection);
    }

    // MAP: chathistory_index: session_id
    QMap<int, std::string> chathistory_index_to_session_id;

    // 获取历史记录并初始化左侧导航栏
    // 加载会话记录的标题（第一条消息）
    void updateHistoryList() {
        // clear history list
        historyList->clear();

        // 获取历史消息的第一条记录
        std::string firstMessage = client->get_first_messages();
        if (firstMessage != "null") {
            // json format: key: username/sessionid, value: "firstmessage"
            auto json_data = QJsonDocument::fromJson(QString::fromStdString(firstMessage).toUtf8());
            QJsonObject jsonObj = json_data.object();  // 转换为 QJsonObject
            
            for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
                QString key = it.key();
                QString value = it.value().toString();
                addNewHistoryItem(value);
                chathistory_index_to_session_id[historyList->count() - 1] = key.toStdString();
            }
        }
    }
    // 添加历史记录项
    void addNewHistoryItem(const QString &title) {
        QListWidgetItem *item = new QListWidgetItem(title);
        historyList->addItem(item);
    }
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QNChatMessage::User_Type type)
    {
        // 计算消息的实际大小
        QSize size = messageW->fontRect(text + "  ");
        messageW->setFixedWidth(this->width());
        item->setSizeHint(size);
        messageW->setText(text, size, type);
        chatList->setItemWidget(item, messageW);
    }
    // 添加消息到聊天窗口
    void addMessage(QListWidget *chatList, const QString &message, bool isUser) {
        welcomeLabel->hide();  // 添加这一行
        QString messageText = message;
        // 移除最后的"<exit>"
        messageText.remove(QRegularExpression("\\n*<exit>$"));  // 移除最后的\n<exit>
        messageText.remove(QRegularExpression("<exit>$"));  // 移除最后的<exit>

        QNChatMessage* messageW = new QNChatMessage(chatList);
        QListWidgetItem* item = new QListWidgetItem(chatList);
        dealMessage(messageW, item, messageText, isUser ? QNChatMessage::User_Me : QNChatMessage::User_She);
        messageW->setTextSuccess();
    }

    void resizeEvent(QResizeEvent *event)
    {
        Q_UNUSED(event);

        // messageInput->resize(this->width() - 20, this->height() - 20);
        // messageInput->move(10, 10);

        // sendButton->move(messageInput->width()+messageInput->x() - sendButton->width() - 10,
        //                      messageInput->height()+messageInput->y() - sendButton->height() - 10);

        for(int i = 0; i < chatList->count(); i++) {
            QNChatMessage* messageW = (QNChatMessage*)chatList->itemWidget(chatList->item(i));
            QListWidgetItem* item = chatList->item(i);

            dealMessage(messageW, item, messageW->text(), messageW->userType());
        }
    }

    // 加载指定会话的聊天记录
    void loadChatHistory(int index) {
        chatList->clear();
        updateWelcomeLabel();  // 添加这一行
        session_id = chathistory_index_to_session_id[index];
        std::string json_str = client->get_chat_history(session_id);
        
        if (json_str != "null") {
            try {
                // 解析JSON
                auto json_obj = json::parse(json_str);
                
                // 检查history是否为数组
                if (json_obj["history"].is_array()) {
                    auto chat_history = json_obj["history"];
                    
                    for (const auto& message : chat_history) {
                        // 假设每个消息对象包含role和content字段
                        if (message.contains("role") && message.contains("content")) {
                            std::string role = message["role"];
                            std::string content = message["content"];
                            QString content_qstr = QString::fromStdString(content);
                            
                            addMessage(chatList, content_qstr, role == "user");
                        }
                    }
                }
            } catch (const json::exception& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
                // 可以在这里添加用户提示
            }
        }
    }

    void sendMessageAsync(const QString &message) {
        // 添加用户消息到当前会话
        addMessage(chatList, message, true);
        messageInput->clear();

        // 创建新线程处理网络请求
        QFuture<void> future = QtConcurrent::run([this, message]() {
            try {
                // 在新线程中发送消息
                std::string response = client->send_message(session_id, message.toStdString());
                json response_json = json::parse(response);
                std::string response_content = response_json["response"];

                // 使用信号将响应发送回主线程
                emit messageReceived(QString::fromStdString(response_content));
            } catch (const std::exception& e) {
                emit errorOccurred(QString("Error: %1").arg(e.what()));
            }
        });
    }

    void sendMessage() {
        QString message = messageInput->toPlainText().trimmed();
        if (!message.isEmpty()) {
            sendMessageAsync(message);
        }
    }

    signals:
        void backToWelcomeWindow(); // 信号用于切换回 WelcomeWindow
        void messageReceived(const QString &message);
        void errorOccurred(const QString &error);

private slots:
    void handleReceivedMessage(const QString &response) {
        // 在主线程中更新UI
        addMessage(chatList, response, false);
        updateHistoryList();
    }
    
    void handleError(const QString &error) {
        QMessageBox::warning(this, "Error", error);
    }

    void updateWelcomeLabel() {
        if (chatList->count() == 0) {
            welcomeLabel->show();
        } else {
            welcomeLabel->hide();
        }
    }

private:
    QListWidget *historyList;
    QListWidget *chatList;
    QPushButton *sendButton;
    CustomTextEdit *messageInput;  // 将 messageInput 改为自定义的 CustomTextEdit，以便多行输入和处理 Enter 键
    QMap<int, QList<QString>> sessionHistory;  // 保存每个会话的消息
    Client *client;             // 指向 Client 的指针
    std::string session_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();  // 当前会话 ID
    int currentSessionIndex;  // 当前会话索引
    QLabel *welcomeLabel;  // 添加这一行到private部分
};
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // 修改构造函数，允许传递 Client 实例
    MainWindow(Client *client, QWidget *parent = nullptr) : QMainWindow(parent), client(client) {
        auto *stackedWidget = new QStackedWidget(this);

        // 创建 WelcomeWindow 和 ChatWindow
        WelcomeWindow *welcomeWindow = new WelcomeWindow;
        ChatWindow *chatWindow = new ChatWindow(client); // 将传递的 Client 实例传递给 ChatWindow

        // 将窗口添加到 QStackedWidget 中
        stackedWidget->addWidget(welcomeWindow);
        stackedWidget->addWidget(chatWindow);

        setCentralWidget(stackedWidget);

        // 默认显示 WelcomeWindow
        stackedWidget->setCurrentWidget(chatWindow);

        // 连接信号和槽，切换界面
        connect(chatWindow, &ChatWindow::backToWelcomeWindow, [stackedWidget, welcomeWindow]() {
            stackedWidget->setCurrentWidget(welcomeWindow);
        });  // 切换到迎窗口

        connect(welcomeWindow, &WelcomeWindow::goToChatWindow, [stackedWidget, chatWindow]() {
            stackedWidget->setCurrentWidget(chatWindow);
        });  // 切换到聊天窗口

        setFixedSize(1200, 900);
    }

private:
    Client *client; // 存储传递的 Client 实例
};

// 添加新的 ServerSettings 对话框类
class ServerSettingsDialog : public QDialog {
    Q_OBJECT

public:
    ServerSettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Server Settings");
        setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        
        // Host 设置
        auto *hostLayout = new QHBoxLayout();
        auto *hostLabel = new QLabel("Host:", this);
        hostLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
        hostLabel->setFixedWidth(100);
        hostEdit = new QLineEdit(this);
        hostEdit->setText("night.betterspace.top");
        hostEdit->setStyleSheet(
            "QLineEdit {"
            "   padding: 8px;"
            "   font-size: 14px;"
            "   border: 2px solid #ccc;"
            "   border-radius: 10px;"
            "   background-color: white;"
            "}"
        );
        hostLayout->addWidget(hostLabel);
        hostLayout->addWidget(hostEdit);
        
        // Port 设置
        auto *portLayout = new QHBoxLayout();
        auto *portLabel = new QLabel("Port:", this);
        portLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
        portLabel->setFixedWidth(100);
        portEdit = new QLineEdit(this);
        portEdit->setText("8080");
        portEdit->setStyleSheet(
            "QLineEdit {"
            "   padding: 8px;"
            "   font-size: 14px;"
            "   border: 2px solid #ccc;"
            "   border-radius: 10px;"
            "   background-color: white;"
            "}"
        );
        portLayout->addWidget(portLabel);
        portLayout->addWidget(portEdit);

        // 按钮布局
        auto *buttonLayout = new QHBoxLayout();
        auto *confirmButton = new QPushButton("Apply", this);
        auto *cancelButton = new QPushButton("Cancel", this);
        
        QString buttonStyle = 
            "QPushButton {"
            "   padding: 8px 20px;"
            "   font-size: 14px;"
            "   border-radius: 10px;"
            "   color: black;"
            "}"
            "QPushButton:hover { background-color: #0066b3; }";

        confirmButton->setStyleSheet(buttonStyle + "background-color: #0078d7;" + "color: white;");
        cancelButton->setStyleSheet(buttonStyle + "background-color: #666666;" + "color: white;");
        
        buttonLayout->addWidget(confirmButton);
        buttonLayout->addWidget(cancelButton);
        
        // 添加所有布局
        mainLayout->addLayout(hostLayout);
        mainLayout->addLayout(portLayout);
        mainLayout->addSpacing(20);
        mainLayout->addLayout(buttonLayout);
        setFixedSize(400, 200);
        
        // 连接信号
        connect(confirmButton, &QPushButton::clicked, this, &ServerSettingsDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &ServerSettingsDialog::reject);
    }

    QString getHost() const { return hostEdit->text(); }
    int getPort() const { return portEdit->text().toInt(); }

private:
    QLineEdit *hostEdit;
    QLineEdit *portEdit;
};

// 修改 LoginWindow 类，移除原有的服务器设置相关代码
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(Client *client, QWidget *parent = nullptr)
        : QWidget(parent), client(client) {  // 使用传入的 Client 指针
        setWindowTitle("Login");
        setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(300, 300, 300, 350);  // 左右边距减小

        // 用户名输入
        auto *usernameLayout = new QHBoxLayout();
        auto *usernameLabel = new QLabel("Username:", this);
        usernameLabel->setFixedWidth(150);  // 增加宽度到100
        usernameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        // 添加样式以处理文本溢出
        usernameLabel->setStyleSheet(
            "QLabel {"
            "   color: white;"
            "   font-size: 18px;"  // 增大字体
            "   font-weight: bold;"
            "   padding-right: 10px;"
            "   min-width: 150px;"
            "}"
        );
        usernameEdit = new QLineEdit(this);
        usernameEdit->setFixedHeight(50);  
        usernameEdit->setMinimumWidth(400);  // 设置最小宽度
        usernameEdit->setStyleSheet(
            "QLineEdit {"
            "   padding: 12px;"
            "   font-size: 16px;"
            "   border: 2px solid #ccc;"
            "   border-radius: 15px;"
            "   background-color: white;"
            "   min-width: 400px;"  // 设置最小宽度
            "}"
            "QLineEdit:focus {"
            "   border: 3px solid #0078d7;"
            "   background-color: #f0f8ff;"
            "}"
        );
        usernameLayout->addWidget(usernameLabel);
        usernameLayout->addWidget(usernameEdit);

        // 密码输入
        QHBoxLayout *passwordLayout = new QHBoxLayout();
        QLabel *passwordLabel = new QLabel("Password:", this);
        passwordLabel->setFixedWidth(150);  // 增加宽度到100
        passwordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        // 使用相同的样式
        passwordLabel->setStyleSheet(
            "QLabel {"
            "   color: white;"
            "   font-size: 18px;"  // 增大字体
            "   font-weight: bold;"
            "   padding-right: 10px;"
            "   min-width: 150px;"
            "}"
        );
        passwordEdit = new QLineEdit(this);
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setFixedHeight(50);
        passwordEdit->setMinimumWidth(400);  // 设置最小宽度
        passwordEdit->setStyleSheet(
            "QLineEdit {"
            "   padding: 12px;"
            "   font-size: 16px;"
            "   border: 2px solid #ccc;"
            "   border-radius: 15px;"
            "   background-color: white;"
            "   min-width: 400px;"  // 设置最小宽度
            "}"
            "QLineEdit:focus {"
            "   border: 3px solid #0078d7;"
            "   background-color: #f0f8ff;"
            "}"
        );
        passwordLayout->addWidget(passwordLabel);
        passwordLayout->addWidget(passwordEdit);

        // 设置布局间距
        usernameLayout->setSpacing(20);  // 增加标签和输入框之间的间距
        passwordLayout->setSpacing(20);  // 增加标签和输入框之间的间距
        mainLayout->setSpacing(30);      // 增加各行之间的间距

        // 登录按钮样式美化
        loginButton = new QPushButton("Login", this);
        loginButton->setFixedSize(200, 50);  // 增大按钮尺寸
        loginButton->setStyleSheet(
            "QPushButton {"
            "   font-size: 18px;"  // 增大字体
            "   font-weight: bold;"
            "   color: white;"
            "   background-color: #0078d7;"
            "   border: none;"
            "   border-radius: 25px;"  // 增加圆角
            "}"
            "QPushButton:hover {"
            "   background-color: #0066b3;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #005299;"
            "}"
        );
        loginButton->move(500, 680);  // 调整按钮位置

        mainLayout->addLayout(usernameLayout);
        mainLayout->addLayout(passwordLayout);
        mainLayout->addSpacing(20);

        // 设置标签字体样式
        QString labelStyle = "QLabel { color: white; font-size: 14px; font-weight: bold; }";
        usernameLabel->setStyleSheet(labelStyle);
        passwordLabel->setStyleSheet(labelStyle);

        setFixedSize(1200, 900);

        connect(loginButton, &QPushButton::clicked, this, &LoginWindow::validateLogin);
    }

private slots:
    void validateLogin() {
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();

        // 尝试登录前先测试服务器连接
        if (!testServerConnection()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Server Connection Failed",
                "Cannot connect to server. Would you like to configure server settings?",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                ServerSettingsDialog dialog(this);
                if (dialog.exec() == QDialog::Accepted) {
                    // 用户点击了确认按钮
                    QString host = dialog.getHost();
                    int port = dialog.getPort();
                    if (!host.isEmpty() && port > 0) {
                        client = new Client(host.toStdString(), port);
                    }
                }
                return;
            }
            return;
        }

        // 服务器连接正常，进行实际登录
        if (client->login(username.toStdString(), password.toStdString())) {
            QMessageBox::information(this, "Login Successful", "Welcome!");
            openMainWindow();
        } else {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        }
    }

    void openMainWindow() {
        this->hide();  // 隐藏登录窗口
        auto *mainWindow = new MainWindow(client);
        mainWindow->show();
    }

    // 添加测试服务器连接的方法
    bool testServerConnection() {
        try {
            // 使用一个快速失败的测试请求来检查服务器连接
            return client->test_connection();  // 需要在 Client 类中添加此方法
        } catch (...) {
            return false;
        }
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::gray));
        QPixmap mPixmap = QPixmap(":/img/bkg.png");
        painter.drawPixmap(QRect(0, 0, 1200, 900), mPixmap);
    }

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    Client *client;  // 使用指针而不是成员变量
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Client client("localhost", 8080);
    LoginWindow loginWindow(&client);
    loginWindow.show();


    // 模拟登录
    // std::string username = "123090490";
    // std::string password = "SXH2005sxh@ZL";

    // if (client.login(username, password)) {
    //     std::cout << "Login successfully!" << std::endl;

    //     // 发送消息
    //     std::string session_id = "session_user123";
    //     std::string response = client.send_message(session_id, "Hello, Server!");
    //     std::cout << "Server response: " << response << std::endl;

    //     // 获取聊天记录
    //     std::string chat_history = client.get_chat_history(session_id);
    //     std::cout << "Chat history: " << chat_history << std::endl;

    //     // 获取所有会话的第一条消息
    //     std::string first_messages = client.get_first_messages();
    //     std::cout << "All first messages: " << first_messages << std::endl;
    // } else {
    //     std::cout << "Login failed!" << std::endl;
    // }

    return app.exec();
}
#include "main.moc"
