#pragma once
#include "shared_ptr.hpp"

namespace my {

template <typename T>
class weak_ptr {
    friend class shared_ptr<T>;

public:
    weak_ptr() noexcept = default;

    weak_ptr(const shared_ptr<T>& other) noexcept {
        if (other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->weakRefs_++;
        } else
            return;
    }

    weak_ptr(const weak_ptr<T>& other) noexcept {
        if (other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->weakRefs_++;
        } else
            return;
    }

    weak_ptr(weak_ptr<T>&& other) noexcept {
        if (other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            other.ptr_ = nullptr;
            other.ptrToControlBlock_ = nullptr;
        } else
            return;
    }

    // ~weak_ptr() noexcept {
    //     if (ptrToControlBlock_) {
    //         ptrToControlBlock_->decrementWeakRefs();
    //         if (ptrToControlBlock_->getSharedRefs() == 0 && ptrToControlBlock_->getWeakRefs() == 0) {
    //             delete ptrToControlBlock_;
    //         }
    //     }
    // }

    weak_ptr& operator=(weak_ptr<T>& other) noexcept {
        if (ptr_ == other.ptr_ || (!ptr_ && !other.ptr_)) {
        } else if (!ptr_ && other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->weakRefs_++;
        } else if (ptr_ && !other.ptr_) {
            ptrToControlBlock_->weakRefs_--;
            ptr_ = nullptr;
            other.ptrToControlBlock_ = nullptr;
        } else if (ptr_ && other.ptr_) {
            ptrToControlBlock_->weakRefs_--;
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->weakRefs_++;
        }
        return *this;
    }

    // weak_ptr<T>& operator=(const shared_ptr<T>& other) noexcept {
    //     if (other.ptr_ != this && other.ptr_ != nullptr) {
    //         ptr_ = other.ptr_;
    //         ptrToControlBlock_ = other.ptrToControlBlock_;
    //         ptrToControlBlock_->incrementSharedRefs();
    //     } else if (other.ptr_ != this && other.ptr_ == nullptr) {
    //         ptr_ = nullptr;
    //         ptrToControlBlock_ = nullptr;
    //     }
    // }

    int use_count() const noexcept {
        if (ptrToControlBlock_) {
            return ptrToControlBlock_->sharedRefs_;
        } else
            return 0;
    }

    // bool expired() const noexcept {
    //     if (ptrToControlBlock_) {
    //         return ptrToControlBlock_->getSharedRefs() == 0;
    //     }
    //     return true;
    // }

    shared_ptr<T> lock() const noexcept {
        if (ptr_) {
            ptrToControlBlock_->sharedRefs_++;
            return shared_ptr<T>(ptr_, ptrToControlBlock_);
        } else {
            return shared_ptr<T>(nullptr);
        }
    }

    // void reset() noexcept {
    //     if (ptr_) {
    //         ptrToControlBlock_->decrementWeakRefs();
    //     }
    //     ptr_ = nullptr;
    //     ptrToControlBlock_ = nullptr;
    // }

private:
    T* ptr_{nullptr};
    typename shared_ptr<T>::controlBlock* ptrToControlBlock_{nullptr};
};

}  // namespace my