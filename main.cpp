#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("Step 1: Hello Qt");
    window.resize(400, 300);
    window.show();
    return app.exec();
}