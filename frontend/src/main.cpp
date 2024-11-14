#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // 创建 Qt 应用实例

    QWidget window;  // 创建主窗口
    window.setWindowTitle("Qt 示例应用");

    // 创建一个按钮并连接点击事件
    QPushButton *button = new QPushButton("点击我", &window);
    QObject::connect(button, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "消息", "按钮被点击了!");
    });

    window.setFixedSize(300, 200);  // 设置窗口大小
    window.show();  // 显示窗口

    return app.exec();  // 启动事件循环
}
