#pragma once
#include "Typing.hpp"
#include <ImageGraph/core/Tile.hpp>
#include <QtCore/QObject>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsScene>
#include <stdexcept>

/**
 * @brief A trivial QObject that can be used to link its signals to arbitrary slots with the correct arguments.
 */
struct MagicObject : public QObject {
  Q_OBJECT

signals:
  void boolSignal(bool) const;
  void voidSignal() const;
  void tileSignal(tile_u8_t) const;
};

struct MagicGraphicsScene : public QGraphicsScene {
  Q_OBJECT

public slots:
  void addTile(tile_u8_t tile) {
    uint8_t* data{tile->data()};
    const auto channels{tile->channels()};
    QImage::Format format;
    switch (channels) {
      case 1: format = QImage::Format_Grayscale8; break;
      case 3: format = QImage::Format_RGB888; break;
      case 4: format = QImage::Format_RGBA8888; break;
      default: throw std::invalid_argument("The tile does not hat 1, 3, or 4 channels!");
    }
    const auto width{tile->width()};
    auto* item{
        addPixmap(QPixmap::fromImage(QImage(data, int(width), int(tile->height()), int(channels * width), format)))};
    item->setOffset(qreal(tile->point().x()), qreal(tile->point().y()));
  }

public:
  MagicGraphicsScene(QObject* parent = nullptr) : QGraphicsScene{parent} {}
};
