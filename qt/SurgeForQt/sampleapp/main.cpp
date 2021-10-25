#include <MainViewModel.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <StacktracePrinter.h>

void loadView(QQmlApplicationEngine *engine, ViewModel *viewModel) {
    QQmlContext* rootContex = engine->rootContext();
    rootContex->setContextProperty("viewModel",  QVariant::fromValue(viewModel));
    engine->load(viewModel->qmlLocation);
}

int main(int argc, char *argv[]){
    registerStacktracePrinterOnSegFault();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    MainViewModel *viewModel = new MainViewModel();

    loadView(&engine, viewModel);

    return app.exec();
}
