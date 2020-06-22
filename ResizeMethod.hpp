#pragma once
#include <QtCore/QMetaType>
#include <QtCore/QString>

enum class ResizeMethod { NEAREST, BLOCK, BILINEAR, BICUBIC, LANCZOS };
static QString toString(ResizeMethod method) {
  switch (method) {
    case ResizeMethod::NEAREST: return QStringLiteral("Nearest Neighbour");
    case ResizeMethod::BLOCK: return QStringLiteral("Block");
    case ResizeMethod::BILINEAR: return QStringLiteral("Bilinear");
    case ResizeMethod::BICUBIC: return QStringLiteral("Bicubic");
    case ResizeMethod::LANCZOS: return QStringLiteral("Lanczos");
  }
  throw std::invalid_argument("The switch has not worked!");
}
Q_DECLARE_METATYPE(ResizeMethod)
