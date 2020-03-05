#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QtWebEngine>

QString APP_NAME = "pi-top Web UI Viewer";

bool isPi()
{
#ifdef __arm__
  return true;
#else
  return false;
#endif
}

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QtWebEngine::initialize();

  QQmlApplicationEngine engine;
  if (isPi())
  {
    engine.load(QUrl(QStringLiteral("/usr/lib/pt-web-ui/pt-web-ui.qml")));
  }
  else
  {
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  }

  if (engine.rootObjects().isEmpty())
  {
    return -1;
  }
  QObject *rootObject = engine.rootObjects().constFirst();

  ////////////////////////////////
  // SET UP COMMAND LINE PARSER //
  ////////////////////////////////

  QCommandLineParser parser;
  parser.setApplicationDescription(APP_NAME);
  // parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption titleOption(QStringList() << "t"
                                               << "title",
                                 "Title of window", "title", "");
  parser.addOption(titleOption);

  QCommandLineOption urlOption(QStringList() << "u"
                                             << "url",
                               "URL to point to", "url", "");
  parser.addOption(urlOption);

  QCommandLineOption fullScreenOption(
      QStringList() << "f"
                    << "fullscreen",
      "Start the app in fullscreen mode - ignores height and width");
  parser.addOption(fullScreenOption);

  QCommandLineOption widthOption(
      QStringList() << "w"
                    << "width",
      "Window width (relative to screen, 0-1). Defaults to 1.0.", "width", "");
  parser.addOption(widthOption);

  QCommandLineOption heightOption(
      QStringList() << "h"
                    << "height",
      "Window height (relative to screen, 0-1). Defaults to 1.0.", "height",
      "");
  parser.addOption(heightOption);

  parser.process(app);

  ///////////////////////////////
  // PROCESS COMMAND LINE ARGS //
  ///////////////////////////////

  QString title = APP_NAME;
  const QString &providedTitle = parser.value(titleOption);
  if (providedTitle != "")
  {
    title = providedTitle;
  }
  app.setApplicationName(title);
  rootObject->setProperty("title", title);

  // app.setWindowIcon(QIcon("/path/to/" + title + ".png"));

  rootObject->setProperty("url", parser.value(urlOption));

  if (parser.isSet(fullScreenOption))
  {
    rootObject->setProperty("visibility", "FullScreen");
  }
  else
  {
    rootObject->setProperty("visibility", "Windowed");
  }

  const QSize &screenSize = app.primaryScreen()->size();

  const QString &widthScaleStr = parser.value(widthOption);
  float widthScalingFactor;
  bool validWidth = true;
  if (widthScaleStr == "")
  {
    widthScalingFactor = 1.0;
  }
  else
  {
    widthScalingFactor = widthScaleStr.toFloat(&validWidth);
  }

  if (validWidth)
  {
    rootObject->setProperty("width", widthScalingFactor * screenSize.width());
  }
  else
  {
    qFatal("Invalid width specified");
  }

  const QString &heightScaleStr = parser.value(heightOption);
  float heightScalingFactor;
  bool validHeight = true;
  if (heightScaleStr == "")
  {
    heightScalingFactor = 1.0;
  }
  else
  {
    heightScalingFactor = heightScaleStr.toFloat(&validHeight);
  }

  if (validHeight)
  {
    rootObject->setProperty("height",
                            heightScalingFactor * screenSize.height());
  }
  else
  {
    qFatal("Invalid height specified");
  }

  rootObject->setProperty("initialised", true);

  ///////////
  // START //
  ///////////

  return app.exec();
}
