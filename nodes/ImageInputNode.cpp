#include "ImageInputNode.hpp"
#include <ImageGraph/core/nodes/impl/Vips.hpp>

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

void ImageButton::init() {
  connect(this, &QPushButton::clicked, [this](bool) {
    QUrl home{QUrl::fromLocalFile(QDir::home().absolutePath())};
    QUrl url{QFileDialog::getOpenFileUrl(nullptr, "Load image", home, "Image file (*.jpg *.jpeg *.png)", nullptr)};
    std::cout << "isEmpty: " << url.isEmpty() << "; isLocalFile: " << url.isLocalFile() << std::endl;
    if (url.isEmpty() or not url.isLocalFile()) return;
    url_ = std::move(url);
    updateValue();
  });
}

void ImageInputNode::initializeUrls() { updateUrl(this->innerWidget()->innerWidget()->url()); }

using wrap_t = ImageGraph::nodes::UniqueVipsWrap;
using unique_t = std::unique_ptr<ImageGraph::OutNode>;

struct VipsPerformer {
  template<typename T> static inline unique_t perform(wrap_t wrap) {
    auto load_node{std::make_unique<ImageGraph::nodes::VipsInputOutputNode<T>>(std::move(wrap))};
    return load_node->isValid() ? std::move(load_node) : nullptr;
  }
};

bool ImageInputNode::updateUrl(const QUrl& url) {
  using namespace ImageGraph;
  using namespace ImageGraph::nodes;

  this->preGeometryChange();
  std::cout << "update url: " << url.toLocalFile().toStdString() << std::endl;
  auto wrap{wrap_t::loadFromFile(url.toLocalFile().toStdString())};
  auto* port{this->outPort()};
  port->valueWillUpdate();
  node_.reset();
  if (wrap.isValid()) {
    if (unique_t new_node{perform_from_vips<VipsPerformer, unique_t>(wrap.format(), std::move(wrap)).value()}) {
      auto* raw_node{new_node.get()};
      node_.replaceNode(std::move(new_node));
      port->updateValue(raw_node);
      setState(NodeState::NORMAL);
      return true;
    }
  }
  port->updateValue(nullptr);
  setState(NodeState::INVALID_ARGUMENTS);
  return false;
}

std::array<ConnectionWrapper, 2> ImageInputNode::createConnections() {
  return {ConnectionWrapper{QObject::connect(this->innerWidget()->innerWidget(), &__detail::ImageButton::urlChanged,
                                             [this](QUrl url) {
                                               changed_ = true;
                                               updateUrl(url);
                                             })},
          ConnectionWrapper{QObject::connect(this->typedView(), &ImageGraphView::changesApplied, [this] {
            const auto now{time::clock_t::now()};
            if (changed_) {
              points_.insert(now);
              changed_ = false;
            }
            if (node_) node_.get()->setChangeProbability(points_.computeRelevance(now));
          })}};
}
