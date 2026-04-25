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
    events_.push_back(event);
  }

  void Tick() {
    current_time_++;
    
    // Don't process if we've exceeded the duration
    if (current_time_ > duration_) {
      return;
    }
    
    for (const Event *event : events_) {
      if (event->IsComplete()) {
        continue;
      }
      
      // Check in order: most common types first for performance
      if (const NormalEvent *ne = dynamic_cast<const NormalEvent*>(event)) {
        // NormalEvent
        if (current_time_ == ne->GetDeadline()) {
          std::cout << ne->GetNotification(0) << std::endl;
        }
      } else if (const CustomNotifyLateEvent *cnle = dynamic_cast<const CustomNotifyLateEvent*>(event)) {
        // CustomNotifyLateEvent (must check before NotifyLateEvent)
        int deadline = cnle->GetDeadline();
        int frequency = cnle->GetFrequency();
        
        if (current_time_ == deadline) {
          std::cout << cnle->GetNotification(0) << std::endl;
        } else if (current_time_ > deadline && (current_time_ - deadline) % frequency == 0) {
          int n = (current_time_ - deadline) / frequency;
          std::cout << cnle->GetNotification(n) << std::endl;
        }
      } else if (const NotifyLateEvent *nle = dynamic_cast<const NotifyLateEvent*>(event)) {
        // NotifyLateEvent
        int deadline = nle->GetDeadline();
        int frequency = nle->GetFrequency();
        
        if (current_time_ == deadline) {
          std::cout << nle->GetNotification(0) << std::endl;
        } else if (current_time_ > deadline && (current_time_ - deadline) % frequency == 0) {
          int n = (current_time_ - deadline) / frequency;
          std::cout << nle->GetNotification(n) << std::endl;
        }
      } else if (const NotifyBeforeEvent *nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
        // NotifyBeforeEvent
        int notify_time = nbe->GetNotifyTime();
        int deadline = nbe->GetDeadline();
        
        if (current_time_ == notify_time) {
          std::cout << nbe->GetNotification(0) << std::endl;
        } else if (current_time_ == deadline) {
          std::cout << nbe->GetNotification(1) << std::endl;
        }
      }
    }
  }

 private:
  int duration_;
  int current_time_;
  std::vector<const Event*> events_;
};

#endif
