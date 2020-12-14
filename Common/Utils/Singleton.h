/**
 * @note スレッドセーフではありません。
 * 各自、mutex など用いてください。
 */

#pragma once

#include <memory>

/**
 * @brief シングルトンの基底クラス
 */
template <class T> class Singleton {
public:
  /**
   * @brief  インスタンスの生成
   * @tparam ... Args        可変長テンプレートパラメータ
   * @param  Args&&... args  コンストラクタの引数
   */
  template <class... Args> static T &Get(Args &&... args) {
    if (m_instance == nullptr) {
      Create(std::forward<Args>(args)...);
    }
    return *m_instance.get();
  }

  /**
   * @brief  インスタンスの生成
   * @tparam ... Args        可変長テンプレートパラメータ
   * @param  Args&&... args  コンストラクタの引数
   */
  template <class... Args> static void Create(Args &&... args) {
    if (m_instance != nullptr) {
      return;
    }
    m_instance = std::make_unique<T>(std::forward<Args>(args)...);
  }

  /**< @brief インスタンスの破棄 */
  static void Destroy() {
    if (m_instance != nullptr) {
      decltype(auto) res = m_instance.release();
      delete res;
      m_instance = nullptr;
    }
  }

  /**< @brief インスタンスが存在するか判定 */
  static bool IsExist() { return m_instance != nullptr; }

protected:
  // --------------------------------------------------------------------------------
  // 特殊メンバ関数
  // --------------------------------------------------------------------------------

  explicit Singleton<T>() = default;  /**< @brief コンストラクタ */
  ~Singleton<T>() noexcept = default; /**< @brief デストラクタ   */

private:
  Singleton<T>(const Singleton<T> &) = delete;
  Singleton<T> &operator=(const Singleton<T> &) = delete;

  // --------------------------------------------------------------------------------
  // 静的メンバ変数
  // --------------------------------------------------------------------------------

  static inline std::unique_ptr<T> m_instance =
      nullptr; /**< インスタンス本体   */
};
