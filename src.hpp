#ifndef MEMO_HPP
#define MEMO_HPP

#include "event.h"
#include <vector>
#include <iostream>

// Implementation of CustomNotifyLateEvent::GetNotification
inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

// Memo class
class Memo {
 public:
  Memo() = delete;

  Memo(int duration) : duration_(duration), current_time_(0) {}

  ~Memo() {
    // No need to delete events as we don't own them
  }

  void AddEvent(const Event *event) {
    // Store events in type-specific vectors for faster access
    if (dynamic_cast<const NormalEvent*>(event)) {
      normal_events_.push_back(event);
    } else if (dynamic_cast<const CustomNotifyLateEvent*>(event)) {
      custom_late_events_.push_back(event);
    } else if (dynamic_cast<const NotifyLateEvent*>(event)) {
      late_events_.push_back(event);
    } else if (dynamic_cast<const NotifyBeforeEvent*>(event)) {
      before_events_.push_back(event);
    }
  }

  void Tick() {
    current_time_++;
    
    // Don't process if we've exceeded the duration
    if (current_time_ > duration_) {
      return;
    }
    
    // Process NormalEvents
    for (const Event *event : normal_events_) {
      if (event->IsComplete()) continue;
      if (current_time_ == event->GetDeadline()) {
        std::cout << event->GetNotification(0) << std::endl;
      }
    }
    
    // Process NotifyBeforeEvents
    for (const Event *event : before_events_) {
      if (event->IsComplete()) continue;
      const NotifyBeforeEvent *nbe = static_cast<const NotifyBeforeEvent*>(event);
      int notify_time = nbe->GetNotifyTime();
      int deadline = nbe->GetDeadline();
      
      if (current_time_ == notify_time) {
        std::cout << nbe->GetNotification(0) << std::endl;
      } else if (current_time_ == deadline) {
        std::cout << nbe->GetNotification(1) << std::endl;
      }
    }
    
    // Process NotifyLateEvents
    for (const Event *event : late_events_) {
      if (event->IsComplete()) continue;
      const NotifyLateEvent *nle = static_cast<const NotifyLateEvent*>(event);
      int deadline = nle->GetDeadline();
      int frequency = nle->GetFrequency();
      
      if (current_time_ == deadline) {
        std::cout << nle->GetNotification(0) << std::endl;
      } else if (current_time_ > deadline && (current_time_ - deadline) % frequency == 0) {
        int n = (current_time_ - deadline) / frequency;
        std::cout << nle->GetNotification(n) << std::endl;
      }
    }
    
    // Process CustomNotifyLateEvents
    for (const Event *event : custom_late_events_) {
      if (event->IsComplete()) continue;
      const CustomNotifyLateEvent *cnle = static_cast<const CustomNotifyLateEvent*>(event);
      int deadline = cnle->GetDeadline();
      int frequency = cnle->GetFrequency();
      
      if (current_time_ == deadline) {
        std::cout << cnle->GetNotification(0) << std::endl;
      } else if (current_time_ > deadline && (current_time_ - deadline) % frequency == 0) {
        int n = (current_time_ - deadline) / frequency;
        std::cout << cnle->GetNotification(n) << std::endl;
      }
    }
  }

 private:
  int duration_;
  int current_time_;
  std::vector<const Event*> normal_events_;
  std::vector<const Event*> before_events_;
  std::vector<const Event*> late_events_;
  std::vector<const Event*> custom_late_events_;
};

#endif
