#ifndef MEMO_HPP
#define MEMO_HPP

#include "event.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>

// Implementation of CustomNotifyLateEvent::GetNotification
inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  return NotifyLateEvent::GetNotification(n) + generator_(n);
}

// Memo class
class Memo {
 public:
  Memo() = delete;

  Memo(int duration) : duration_(duration), current_time_(0) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
  }

  ~Memo() {
    // No need to delete events as we don't own them
  }

  void AddEvent(const Event *event) {
    if (const NormalEvent *ne = dynamic_cast<const NormalEvent*>(event)) {
      // Schedule notification at deadline
      schedule_[ne->GetDeadline()].push_back(event);
    } else if (const NotifyBeforeEvent *nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      // Schedule notification at notify_time and deadline
      schedule_[nbe->GetNotifyTime()].push_back(event);
      schedule_[nbe->GetDeadline()].push_back(event);
    } else if (const NotifyLateEvent *nle = dynamic_cast<const NotifyLateEvent*>(event)) {
      // Schedule notification at deadline
      // Late notifications will be handled dynamically in Tick()
      schedule_[nle->GetDeadline()].push_back(event);
      late_events_.insert(event);
    }
  }

  void Tick() {
    current_time_++;
    
    // Don't process if we've exceeded the duration
    if (current_time_ > duration_) {
      return;
    }
    
    // Process scheduled events for current time
    if (schedule_.count(current_time_)) {
      for (const Event *event : schedule_[current_time_]) {
        if (event->IsComplete()) continue;
        
        if (const NormalEvent *ne = dynamic_cast<const NormalEvent*>(event)) {
          std::cout << ne->GetNotification(0) << std::endl;
        } else if (const NotifyBeforeEvent *nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
          if (current_time_ == nbe->GetNotifyTime()) {
            std::cout << nbe->GetNotification(0) << std::endl;
          } else if (current_time_ == nbe->GetDeadline()) {
            std::cout << nbe->GetNotification(1) << std::endl;
          }
        } else if (const CustomNotifyLateEvent *cnle = dynamic_cast<const CustomNotifyLateEvent*>(event)) {
          if (current_time_ == cnle->GetDeadline()) {
            std::cout << cnle->GetNotification(0) << std::endl;
            // Schedule next late notification
            int next_time = current_time_ + cnle->GetFrequency();
            if (next_time <= duration_) {
              schedule_[next_time].push_back(event);
            }
          } else if (current_time_ > cnle->GetDeadline()) {
            int n = (current_time_ - cnle->GetDeadline()) / cnle->GetFrequency();
            std::cout << cnle->GetNotification(n) << std::endl;
            // Schedule next late notification
            int next_time = current_time_ + cnle->GetFrequency();
            if (next_time <= duration_) {
              schedule_[next_time].push_back(event);
            }
          }
        } else if (const NotifyLateEvent *nle = dynamic_cast<const NotifyLateEvent*>(event)) {
          if (current_time_ == nle->GetDeadline()) {
            std::cout << nle->GetNotification(0) << std::endl;
            // Schedule next late notification
            int next_time = current_time_ + nle->GetFrequency();
            if (next_time <= duration_) {
              schedule_[next_time].push_back(event);
            }
          } else if (current_time_ > nle->GetDeadline()) {
            int n = (current_time_ - nle->GetDeadline()) / nle->GetFrequency();
            std::cout << nle->GetNotification(n) << std::endl;
            // Schedule next late notification
            int next_time = current_time_ + nle->GetFrequency();
            if (next_time <= duration_) {
              schedule_[next_time].push_back(event);
            }
          }
        }
      }
    }
  }

 private:
  int duration_;
  int current_time_;
  std::map<int, std::vector<const Event*>> schedule_;
  std::set<const Event*> late_events_;
};

#endif
