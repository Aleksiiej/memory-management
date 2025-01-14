#pragma once
#include <atomic>

namespace my {

template <typename T>
class shared_ptr {
    template <typename Y>
    friend class weak_ptr;

    class controlBlock {
        friend class shared_ptr;
        template <typename Y>
        friend class weak_ptr;

    public:
        constexpr explicit controlBlock() noexcept {}
        controlBlock(const controlBlock&) = delete;
        controlBlock& operator=(const controlBlock&) = delete;
        controlBlock(controlBlock&&) = delete;
        controlBlock& operator=(controlBlock&&) = delete;

    private:
        std::atomic<std::size_t> sharedRefs_{1};
        std::atomic<std::size_t> weakRefs_{0};
        void (*deleter_)(T*) = [](T* ptr_) { delete ptr_; };
    };

public:
    shared_ptr(T* ptr = nullptr) noexcept
        : ptr_(ptr) {
        if (ptr_) {
            ptrToControlBlock_ = new controlBlock;
        } else {
            ptrToControlBlock_ = nullptr;
        }
    }

    shared_ptr(T* ptr, void (*deleter)(T*)) noexcept
        : shared_ptr(ptr) {
        ptrToControlBlock_->deleter_ = deleter;
    }

    shared_ptr(const shared_ptr& other) noexcept
        : ptr_(other.ptr_), ptrToControlBlock_(other.ptrToControlBlock_) {
        if (ptr_) {
            ptrToControlBlock_->sharedRefs_++;
        }
    }

    shared_ptr(shared_ptr&& other) noexcept {
        if (other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            other.ptr_ = nullptr;
            other.ptrToControlBlock_ = nullptr;
        }
    }

    ~shared_ptr() {
        if (ptr_ || ptrToControlBlock_) {
            cleanUp();
        }
    }

    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (ptr_ == other.ptr_ || (!ptr_ && !other.ptr_)) {
        } else if (!ptr_ && other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->sharedRefs_++;
        } else if (ptr_ && !other.ptr_) {
            cleanUp();
            ptr_ = nullptr;
            ptrToControlBlock_ = nullptr;
        } else if (ptr_ && other.ptr_) {
            cleanUp();
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            ptrToControlBlock_->sharedRefs_++;
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept {
        if (ptr_ == other.ptr_ || (!ptr_ && !other.ptr_)) {
        } else if (!ptr_ && other.ptr_) {
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            other.ptr_ = nullptr;
            other.ptrToControlBlock_ = nullptr;
        } else if (ptr_ && !other.ptr_) {
            cleanUp();
            ptr_ = nullptr;
            ptrToControlBlock_ = nullptr;
        } else if (ptr_ && other.ptr_) {
            cleanUp();
            ptr_ = other.ptr_;
            ptrToControlBlock_ = other.ptrToControlBlock_;
            other.ptr_ = nullptr;
            other.ptrToControlBlock_ = nullptr;
        }
        return *this;
    }

    operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T& operator*() const noexcept {
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    T* get() const noexcept {
        return ptr_;
    }

    T* release() noexcept {
        T* tempPtr = std::move(ptr_);
        ptr_ = nullptr;
        return tempPtr;
    }

    void reset(T* ptr) noexcept {
        delete ptr_;
        ptr_ = std::move(ptr);
    }

    void reset() noexcept {
        delete ptr_;
        ptr_ = nullptr;
    }

    int use_count() const noexcept {
        if (!ptrToControlBlock_) {
            return 0;
        } else {
            return static_cast<int>(ptrToControlBlock_->sharedRefs_);
        }
    }

    controlBlock* getControlBlockPtr() const noexcept {
        if (!ptrToControlBlock_) {
            return nullptr;
        } else
            return ptrToControlBlock_;
    }

private:
    T* ptr_{nullptr};
    controlBlock* ptrToControlBlock_{nullptr};

    shared_ptr(T* ptr, controlBlock* ptrToControlBlock) noexcept
        : ptr_(ptr), ptrToControlBlock_(ptrToControlBlock) {
    }

    void cleanUp() {
        ptrToControlBlock_->sharedRefs_--;
        if (ptrToControlBlock_->sharedRefs_ == 0) {
            std::invoke(ptrToControlBlock_->deleter_, ptr_);
            ptr_ = nullptr;
            delete ptrToControlBlock_;
            ptrToControlBlock_ = nullptr;
        }
    }
};

}  // namespace my