/* Copyright Vital Audio, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "application_window.h"

namespace visage {
  ApplicationWindow::ApplicationWindow() = default;

  ApplicationWindow::~ApplicationWindow() {
    removeFromWindow();
  }

  void ApplicationWindow::registerCallbacks() {
    window_->onShow() = [this] { on_show_.callback(); };
    window_->onHide() = [this] { on_hide_.callback(); };
    window_->onWindowContentsResized() = [this] { on_window_contents_resized_.callback(); };
  }

  void ApplicationWindow::show(const Dimension& width, const Dimension& height, void* parent_window) {
    removeFromWindow();
    window_ = createPluginWindow(width, height, parent_window);
    registerCallbacks();
    showWindow(false);
  }

  void ApplicationWindow::show(const Dimension& width, const Dimension& height) {
    show({}, {}, width, height);
  }

  void ApplicationWindow::show(const Dimension& x, const Dimension& y, const Dimension& width,
                               const Dimension& height) {
    removeFromWindow();
    window_ = createWindow(x, y, width, height, decoration_);
    registerCallbacks();
    showWindow(false);
  }

  void ApplicationWindow::showMaximized() {
    static constexpr float kUnmaximizedWidthPercent = 85.0f;

    removeFromWindow();
    window_ = createWindow({}, {}, Dimension::widthPercent(kUnmaximizedWidthPercent),
                           Dimension::heightPercent(kUnmaximizedWidthPercent), decoration_);
    registerCallbacks();
    showWindow(true);
  }

  void ApplicationWindow::hide() {
    if (window_)
      return window_->hide();
  }

  void ApplicationWindow::close() {
    if (window_) {
      removeFromWindow();
      window_ = nullptr;
    }
  }

  bool ApplicationWindow::isShowing() const {
    return window_ && window_->isShowing();
  }

  Point ApplicationWindow::minWindowDimensions() {
    if (window_)
      return window_->minWindowDimensions();
    return { 0, 0 };
  }

  Point ApplicationWindow::maxWindowDimensions() {
    if (window_)
      return window_->maxWindowDimensions();
    return { INT_MAX, INT_MAX };
  }

  void ApplicationWindow::adjustWindowDimensions(unsigned int* width, unsigned int* height,
                                                 bool horizontal_resize, bool vertical_resize) {
    if (!isFixedAspectRatio() || *width == 0 || *height == 0)
      return;

    visage::Point point(static_cast<int>(*width), static_cast<int>(*height));
    visage::Point dimensions = visage::adjustBoundsForAspectRatio(point, minWindowDimensions(),
                                                                  maxWindowDimensions(), aspectRatio(),
                                                                  horizontal_resize, vertical_resize);
    *width = dimensions.x;
    *height = dimensions.y;
  }

  void ApplicationWindow::runEventLoop() {
    window_->runEventLoop();
  }

  void ApplicationWindow::showWindow(bool maximized) {
    if (!title_.empty())
      window_->setWindowTitle(title_);

    addToWindow(window_.get());
    if (maximized)
      window_->showMaximized();
    else
      window_->show();
  }
}
