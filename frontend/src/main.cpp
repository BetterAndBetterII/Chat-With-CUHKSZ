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

// class ChatMessageWidget : public QWidget {
//     Q_OBJECT
//
// public:
//     ChatMessageWidget(const QString &text, const QString &avatarPath, bool isUser = true, QWidget *parent = nullptr)
//         : QWidget(parent) {
//
//         // 设置整体布局
//         QHBoxLayout *mainLayout = new QHBoxLayout(this);
//         mainLayout->setContentsMargins(10, 10, 10, 10);
//         mainLayout->setSpacing(10);
//
//         // 头像
//         QLabel *avatar = new QLabel;
//         avatar->setFixedSize(50, 50);
//         avatar->setStyleSheet("border-radius: 25px;");
//         avatar->setPixmap(QPixmap(avatarPath).scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
//
//         // 气泡文本
//         QLabel *message = new QLabel(text);
//         message->setWordWrap(true);
//         message->setStyleSheet(isUser
//                                ? "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0078d7, stop:1 #005a9e); color: white; border-radius: 12px; padding: 12px;"
//                                : "background-color: #f0f0f0; color: black; border-radius: 12px; padding: 12px;");
//         message->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//         message->setMaximumWidth(500);  // 增加气泡的最大宽度，使其能够容纳更多文字
//
//         // 布局调整
//         if (isUser) {
//             mainLayout->addStretch();
//             mainLayout->addWidget(message);
//             mainLayout->addWidget(avatar);
//         } else {
//             mainLayout->addWidget(avatar);
//             mainLayout->addWidget(message);
//             mainLayout->addStretch();
//         }
//     }
// };

class WelcomeWindow : public QWidget {
    Q_OBJECT

public:
    WelcomeWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Welcome to Chat_With_CUHKSZ!");
        setFixedSize(1000, 750); // 窗口大小与图片比例匹配

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
        QLabel *titleLabel = new QLabel("Welcome to Chat_With_CUHKSZ!", this);
        titleLabel->setStyleSheet("font-size: 60px; font-weight: bold;color:white");
        QLabel *profilePhotoLabel = new QLabel(this);
        profilePhotoLabel->setPixmap(QPixmap(":/img/chat.gif").scaled(60, 60, Qt::KeepAspectRatio)); // 假设图片路径
        profilePhotoLabel->setStyleSheet("border: 1px solid black;");
        profilePhotoLabel->setFixedSize(60, 60);
        topLayout->addWidget(titleLabel);
        topLayout->addStretch(); // 左对齐
        topLayout->addWidget(profilePhotoLabel);

            // 添加顶部横幅到主布局
        mainLayout->addLayout(topLayout);

        // 中部网格布局
        QVBoxLayout *gridLayout = new QVBoxLayout();
        gridLayout->addSpacing(30);
        gridLayout->setContentsMargins(300,70,300,50);
        // 按钮1: bb
        QPushButton *buttonBB = new QPushButton("BB System", this);
        buttonBB->setFixedSize(400, 30);
        buttonBB->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonBB, 0, 0);
        gridLayout->addWidget(buttonBB);

        // 按钮2: sis
        QPushButton *buttonSIS = new QPushButton("SIS", this);
        buttonSIS->setFixedSize(400, 25);
        buttonSIS->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonSIS, 0, 1);
        gridLayout->addWidget(buttonSIS);

        // 按钮3: Chat with CUHKSZ
        QPushButton *buttonChat = new QPushButton("Back to Chat!", this);
        buttonChat->setFixedSize(400, 25);
        buttonChat->setStyleSheet("font-size: 18px; font-weight: bold; background-color: #006699; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonChat, 0, 2, 1, 2); // 跨两列
        gridLayout->addWidget(buttonChat);
        // 按钮4: Booking
        QPushButton *buttonBooking = new QPushButton("Booking", this);
        buttonBooking->setFixedSize(400, 25);
        buttonBooking->setStyleSheet("font-size: 16px; font-weight: bold; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonBooking, 1, 0, 1, 2); // 跨两列
        gridLayout->addWidget(buttonBooking);
        // 按钮5: Off
        QPushButton *buttonOfficial = new QPushButton("Official", this);
        buttonOfficial->setFixedSize(400, 25);
        buttonOfficial->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(buttonOfficial, 1, 2);
        gridLayout->addWidget(buttonOfficial);
        // 按钮6: 语言切换按钮
        QPushButton *buttonLanguage = new QPushButton("Language: ENGLISH", this);
        buttonLanguage->setFixedSize(400, 25);
        buttonLanguage->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border-radius: 10px;");
        // gridLayout->addWidget(buttonLanguage, 1, 3);
        gridLayout->addWidget(buttonLanguage);
        // 按钮7: vpn
        QPushButton *buttonVpn = new QPushButton("VPN", this);
        buttonVpn->setFixedSize(400, 25);
        buttonVpn->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(buttonVpn, 2, 2);
        gridLayout->addWidget(buttonVpn);
        // 按钮8: 空按钮
        QPushButton *button4 = new QPushButton("Others", this);
        button4->setFixedSize(400, 25);
        button4->setStyleSheet("font-size: 16px; background-color: #444444; color: white; border: 1px solid white; border-radius: 10px;");
        // gridLayout->addWidget(button4, 2, 3);
        gridLayout->addWidget(button4);
        // 添加中部布局到主布局
        mainLayout->addLayout(gridLayout);

        // 在 Booking 下方添加两行文字，紧贴 Booking 按钮
        QLabel *bookingDescription1 = new QLabel("结合传统与现代", this);
        QLabel *bookingDescription2 = new QLabel("融汇中国与西方", this);
        bookingDescription1->setAlignment(Qt::AlignCenter);
        bookingDescription2->setAlignment(Qt::AlignCenter);
        bookingDescription1->setStyleSheet("font-size: 24px; font-family: KaiTi; font-style: italic; color: black;");
        bookingDescription2->setStyleSheet("font-size: 24px; font-family: KaiTi; font-style: italic; color: black;");

        // // 添加文字布局
        // QVBoxLayout *bookingTextLayout = new QVBoxLayout();
        // bookingTextLayout->addWidget(bookingDescription1);
        // bookingTextLayout->addWidget(bookingDescription2);
        // bookingTextLayout->setSpacing(5); // 控制两行文字间的间距

        // 将文字放置到button7的正左边
        QHBoxLayout *textAndButtonLayout = new QHBoxLayout();
        textAndButtonLayout->addWidget(bookingDescription1); // 先显示“结合传统与现代”
        textAndButtonLayout->addStretch(); // 居中显示文字
        textAndButtonLayout->addWidget(bookingDescription2); // 然后显示“融汇中国与西方”

        // 将文字添加到grid布局里，位置紧贴booking按钮正下方
        gridLayout->addLayout(textAndButtonLayout); // 放置在第3行，占据4列

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

            } else {
                // 切换回英文
                buttonLanguage->setText("Language: ENGLISH");

                // 恢复其他按钮的文本
                buttonBB->setText("BB System");
                buttonSIS->setText("SIS");
                buttonBooking->setText("Booking");
                buttonOfficial->setText("Official");
            }
        });
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::gray));
        QPixmap mPixmap = QPixmap(":/img/welcome.png");
        QPixmap scaledPixmap = mPixmap.scaled( 1000, 750, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(QRect(0, 0, 1000, 750), scaledPixmap);
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
        setFixedSize(1000,750);
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
        chatList->setFixedSize(702,520);
        chatList->setStyleSheet("background-color: #f9f9f9; border: none; padding: 10px;");
        //设置为列表显示模式
        chatList->setViewMode(QListView::ListMode);
        //屏蔽水平滑动条
        chatList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //设置为像素滚动
        chatList->setHorizontalScrollMode(QListWidget::ScrollPerPixel);
        //设置鼠标左键拖动
        // QScroller::grabGesture(chatList, QScroller::LeftMouseButtonGesture);

        // 消息输入区域
        messageInput = new CustomTextEdit;
        messageInput->setPlaceholderText("Text anything！");
        messageInput->setStyleSheet("background-color: #ffffff; border: 1px solid #ccc; border-radius: 12px; padding: 10px;");
        messageInput->setMinimumHeight(80);
        messageInput->setMaximumHeight(80);
        messageInput->setFixedSize(702,120);
        messageInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QHBoxLayout *hbLayout = new QHBoxLayout;
        QVBoxLayout *vbLayout = new QVBoxLayout;
        sendButton = new QPushButton("Send");
        sendButton->setStyleSheet("background-color: #0078d7; color: white; border-radius: 12px; padding: 10px 20px;");
        sendButton->setFixedSize(80,35);
        hbLayout->addStretch();
        hbLayout->addWidget(sendButton);
        // hbLayout->setMargin(2);
        vbLayout->addStretch();
        vbLayout->addLayout(hbLayout);
        // vbLayout->setMargin(2);
        // 输入区域布局
        messageInput->setLayout(vbLayout);

        QPushButton *backButton = new QPushButton("Other Options", this);
        backButton->setStyleSheet("background-color: #0078d7; color: white; font-size: 18px; border-radius: 10px;");
        backButton->setFixedSize(120,40);
        connect(backButton, &QPushButton::clicked, this, &ChatWindow::backToWelcomeWindow);

        // 主聊天布局
        auto *chatLayout = new QVBoxLayout;
        auto *titleHBoxLayout = new QHBoxLayout;
        titleHBoxLayout->addWidget(new QLabel("Chat with me @_@"));
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
        initializeHistoryList();

        // 会话历史记录保存
        currentSessionIndex = 0;  // 当前会话索引
        // sessionHistory[currentSessionIndex] = QList<QString>();  // 新建一个空会话

        // 添加多个初始的历史记录
        addNewHistoryItem("Text 1");
        addNewHistoryItem("Text 2");
        addNewHistoryItem("Text 3");
        addNewHistoryItem("Text 4");
        addNewHistoryItem("Text 5");

        // 初始化每个会话的聊天记录（仅模拟）
        sessionHistory[0] = QList<QString> { "User: What is Chat with CUHKSZ？", "Chat_With_CUHKSZ: I am a chatbot always ready to help！ I know a lot about CUHKSZ haha.","User: haha!"};
        sessionHistory[1] = QList<QString> { "User: heihei", "Chat_With_CUHKSZ:haha" };
        sessionHistory[2] = QList<QString> { "User: miaomiao", "Chat_With_CUHKSZ: wangwang" };
        sessionHistory[3] = QList<QString> { "User: gaga", "Chat_With_CUHKSZ:gugu" };
        sessionHistory[4] = QList<QString> { "User: sing!", "Chat_With_CUHKSZ:LALALA~" };

        //测试
        // for (int i= 0; i<2;i++)
        // {
        //     QString message = sessionHistory[1][i];
        //     bool isUser = message.startsWith("User:");
        //     addMessage(chatList, message, "", isUser);
        // }

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
            loadSession(index );
        });

        setWindowTitle("Chat_With_CUHKSZ >_<");
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

    // 获取历史记录并初始化左侧导航栏
    void initializeHistoryList() {
        std::string firstMessages = client->get_first_messages();
        if (firstMessages.find("Error:") == std::string::npos)
        {
            QStringList messages = QString::fromStdString(firstMessages).split("\n");

            for (const QString &message : messages) {
                addNewHistoryItem(message.trimmed());
            }
        }
    }
    // 添加新历史记录项
    void addNewHistoryItem(const QString &title) {
        QListWidgetItem *item = new QListWidgetItem(title);
        historyList->addItem(item);
    }
    void dealMessageTime(QString curMsgTime)
    {
        bool isShowTime = false;
        if(chatList->count() > 0) {
            QListWidgetItem* lastItem = chatList->item(chatList->count() - 1);
            QNChatMessage* messageW = (QNChatMessage*)chatList->itemWidget(lastItem);
            int lastTime = messageW->time().toInt();
            int curTime = curMsgTime.toInt();
            qDebug() << "curTime lastTime:" << curTime - lastTime;
            isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
            //        isShowTime = true;
        } else {
            isShowTime = true;
        }
        if(isShowTime) {
            QNChatMessage* messageTime = new QNChatMessage(chatList);
            QListWidgetItem* itemTime = new QListWidgetItem(chatList);

            QSize size = QSize(this->width(), 40);
            messageTime->resize(size);
            itemTime->setSizeHint(size);
            messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
            chatList->setItemWidget(itemTime, messageTime);
        }
    }
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
    {
        QSize size = messageW->fontRect(text);
        messageW->setFixedWidth(this->width());
        item->setSizeHint(size);
        messageW->setText(text, time, size, type);
        chatList->setItemWidget(item, messageW);
    }
    // 添加消息到聊天窗口
    void addMessage(QListWidget *chatList, const QString &message, const QString &avatarPath, bool isUser) {
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        qDebug() << "addMessage" << message << time << chatList->count();
        dealMessageTime(time);
        QNChatMessage* messageW = new QNChatMessage(chatList);
        QListWidgetItem* item = new QListWidgetItem(chatList);
        dealMessage( messageW, item, message, time, isUser?QNChatMessage::User_Me:QNChatMessage::User_She);
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

            dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
        }
    }

    // 加载指定会话的所有消息
    void loadSession(int index) {
        if (sessionHistory.contains(index)) {
            chatList->clear();  // 清空当前聊天列表

            // 获取历史消息的第一条记录
            // std::string firstMessage = client->get_first_messages();
            // QString firstMessageQString = QString::fromStdString(firstMessage);

            // 添加服务器返回的第一条历史记录（模拟）
            // addMessage(chatList, firstMessageQString, ":/img/img_1.png", false);
            addMessage(chatList, "firstMessageQString", ":/img/img_1.png", false);

            // 加载当前会话的其他消息
            const QList<QString> &messages = sessionHistory[index];
            for (const QString &message : messages) {
                // 判断是用户还是ChatGPT消息
                bool isUser = message.startsWith("User:");
                QString avatarPath = isUser ? ":/img/img.png" : ":/img/img_1.png";
                addMessage(chatList, message, avatarPath, isUser);
            }

            currentSessionIndex = index;  // 更新当前会话索引
        }
    }

    // 发送消息的方法
    /*void sendMessage() {
        QString message = messageInput->toPlainText().trimmed();
        if (!message.isEmpty()) {
            // 添加用户消息到当前会话
            addMessage(chatList, message, "/home/yf/Desktop/Workplace/Group-project/Chat-With-CUHKSZ/frontend/resources/picture", true);
            sessionHistory[currentSessionIndex].append("用户: " + message);

            // 模拟 ChatGPT 回复
            QString response =  message;
            addMessage(chatList, response, ":../resources/picture/img_1.png", false);
            sessionHistory[currentSessionIndex].append("Chat_with_CUHKSZ: " + response);

            messageInput->clear();
        }
    }*/
    void sendMessage() {
        QString message = messageInput->toPlainText().trimmed();
        if (!message.isEmpty()) {
            // 添加用户消息到当前会话
            addMessage(chatList, message, ":/img/CustomerCopy.png", true);

            // 使用 Client 的 send_message 方法获取回复
            std::string response = client->send_message(session_id, message.toStdString());

            // 添加服务器返回的回复
            addMessage(chatList, QString::fromStdString(response), ":/img/CustomerService.png", false);

            messageInput->clear();
        }
    }

    signals:
        void backToWelcomeWindow(); // 信号用于切换回 WelcomeWindow

private:
    QListWidget *historyList;
    QListWidget *chatList;
    QPushButton *sendButton;
    CustomTextEdit *messageInput;  // 将 messageInput 改为自定义的 CustomTextEdit，以便多行输入和处理 Enter 键
    QMap<int, QList<QString>> sessionHistory;  // 保存每个会话的消息
    Client *client;             // 指向 Client 的指针
    std::string session_id;     // 会话 ID
    int currentSessionIndex;  // 当前会话索引
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
        stackedWidget->setCurrentWidget(welcomeWindow);

        // 连接信号和槽，切换界面
        connect(chatWindow, &ChatWindow::backToWelcomeWindow, [stackedWidget, welcomeWindow]() {
            stackedWidget->setCurrentWidget(welcomeWindow);
        });

        connect(welcomeWindow, &WelcomeWindow::goToChatWindow, [stackedWidget, chatWindow]() {
            stackedWidget->setCurrentWidget(chatWindow);
        });

        resize(1000, 750);
    }

private:
    Client *client; // 存储传递的 Client 实例
};

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(Client *client, QWidget *parent = nullptr)
        : QWidget(parent), client(client) {  // 使用传入的 Client 指针
        setWindowTitle("Login");
        setFixedSize(800, 400);

        auto *mainLayout = new QVBoxLayout(this);
        // mainLayout->setMargin(150);
        mainLayout->setContentsMargins(200, 100, 200, 150 );
        // 用户名输入
        auto *usernameLayout = new QHBoxLayout();
        auto *usernameLabel = new QLabel("Username:", this);
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::white);
        usernameLabel->setPalette(pe);
        usernameEdit = new QLineEdit(this);
        usernameLayout->addWidget(usernameLabel);
        usernameLayout->addWidget(usernameEdit);

        // 密码输入
        QHBoxLayout *passwordLayout = new QHBoxLayout();
        QLabel *passwordLabel = new QLabel("Password:", this);
        passwordLabel->setPalette(pe);
        passwordEdit = new QLineEdit(this);
        passwordEdit->setEchoMode(QLineEdit::Password);  // 隐藏密码输入
        passwordLayout->addWidget(passwordLabel);
        passwordLayout->addWidget(passwordEdit);

        // 登录按钮
        loginButton = new QPushButton("Login", this);
        loginButton->setStyleSheet("width: 140px;height: 30px;");
        loginButton->move(330, 310);

        mainLayout->addLayout(usernameLayout);
        mainLayout->addLayout(passwordLayout);
        // mainLayout->addWidget(loginButton);

        connect(loginButton, &QPushButton::clicked, this, &LoginWindow::validateLogin);
    }

private slots:
    void validateLogin() {
        // 获取用户输入
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();

        // 调用 Client 的 login 方法
        if (client->login(username.toStdString(), password.toStdString())) {
            QMessageBox::information(this, "Login Successful", "Welcome!");
            openMainWindow();
        } else {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        }
    }

    void openMainWindow() {
        this->hide();  // 隐藏登录窗口
        auto *mainwindow = new MainWindow(client);
        std::cout << "Login success" << std::endl;
        mainwindow->show();
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::gray));
        QPixmap mPixmap = QPixmap(":/img/bkg.png");
        painter.drawPixmap(QRect(0, 0, 800, 400), mPixmap);
    }

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    Client *client;  // 使用指针而不是成员变量
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Client client("night.betterspace.top", 8080);
    LoginWindow loginWindow(&client);
    loginWindow.show();


    // 模拟登录
    std::string username = "123090490";
    std::string password = "SXH2005sxh@ZL";

    if (client.login(username, password)) {
        std::cout << "Login successfully!" << std::endl;

        // 发送消息
        std::string session_id = "session_user123";
        // std::string response = client.send_message(session_id, "Hello, Server!");
        // std::cout << "Server response: " << response << std::endl;

        // 获取聊天记录
        std::string chat_history = client.get_chat_history(session_id);
        std::cout << "Chat history: " << chat_history << std::endl;

        // 获取所有会话的第一条消息
        std::string first_messages = client.get_first_messages();
        std::cout << "All first messages: " << first_messages << std::endl;
    } else {
        std::cout << "Login failed!" << std::endl;
    }

    return app.exec();
}
#include "main.moc"
