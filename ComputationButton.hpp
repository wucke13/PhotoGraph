#include <QtWidgets/QPushButton>

namespace PhotoGraph {
class ComputationButton : public QPushButton {
  Q_OBJECT

  enum class ComputeState { COMPUTE, DISTRIBUTING, CANCEL };
  ComputeState state_;

  void updateText();
  void setState(ComputeState state);

signals:
  void compute();
  void cancel();

public slots:
  void computationFinished();
  void distributionFinished();

public:
  ComputationButton();
};
} // namespace PhotoGraph