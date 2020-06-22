#include "ComputationButton.hpp"

using namespace PhotoGraph;

void ComputationButton::updateText() {
  switch (state_) {
    case ComputeState::COMPUTE: setText(QStringLiteral("Compute")); break;
    case ComputeState::DISTRIBUTING: setText(QStringLiteral("Distributing…")); break;
    case ComputeState::CANCEL: setText(QStringLiteral("Cancel")); break;
  }
}
void ComputationButton::setState(ComputeState state) {
  switch (state) {
    case ComputeState::COMPUTE: assert(state_ == ComputeState::CANCEL); break;
    case ComputeState::DISTRIBUTING: assert(state_ == ComputeState::COMPUTE); break;
    case ComputeState::CANCEL: assert(state_ == ComputeState::DISTRIBUTING); break;
  }
  state_ = state;
  updateText();
}
void ComputationButton::computationFinished() {
  setState(ComputeState::COMPUTE);
  setEnabled(true);
}
void ComputationButton::distributionFinished() {
  setState(ComputeState::CANCEL);
  setEnabled(true);
}

ComputationButton::ComputationButton() : QPushButton{}, state_{ComputeState::COMPUTE} {
  updateText();
  QObject::connect(this, &QPushButton::pressed, this, [this] {
    switch (state_) {
      case ComputeState::COMPUTE: {
        setState(ComputeState::DISTRIBUTING);
        setEnabled(false);
        emit compute();
        break;
      }
      case ComputeState::DISTRIBUTING: {
        setState(ComputeState::CANCEL);
        setEnabled(true);
        break;
      }
      case ComputeState::CANCEL: {
        setEnabled(false);
        emit cancel();
        break;
      }
    }
  });
}
