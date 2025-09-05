#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include <cmath>
#include <functional>
#include <string>
#include <iostream>
#include <algorithm>

// Base class for all UI elements
class UIElement {
public:
    int x = 0, y = 0, width = 0, height = 0;
    bool visible = true;
    
    virtual ~UIElement() = default;
    virtual void draw() = 0;
    virtual void setPosition(int x, int y) { this->x = x; this->y = y; }
    virtual void setSize(int w, int h) { this->width = w; this->height = h; }
    virtual bool isInteractable() const { return false; }
    virtual bool isMouseOver() const { return false; }
    virtual void onClick() {}
};

// Flexible text element
class FlexText : public UIElement {
private:
    std::string text;
    int fontSize;
    Color color;
    bool centered;
    
public:
    FlexText(const std::string& text, int fontSize = 20, Color color = WHITE, bool centered = false)
        : text(text), fontSize(fontSize), color(color), centered(centered) {
        updateSize();
    }
    
    void updateSize() {
        width = MeasureText(text.c_str(), fontSize);
        height = fontSize;
    }
    
    void setText(const std::string& newText) {
        text = newText;
        updateSize();
    }
    
    void setFontSize(int size) {
        fontSize = size;
        updateSize();
    }
    
    void setColor(Color c) { color = c; }
    
    void draw() override {
        if (!visible) return;
        int drawX = centered ? x - width / 2 : x;
        DrawText(text.c_str(), drawX, y, fontSize, color);
    }
};

// Flexible button element
class FlexButton : public UIElement {
private:
    std::string text;
    int fontSize;
    Color bgColor, hoverColor, textColor;
    bool hovered = false;
    std::function<void()> callback;
    
public:
    FlexButton(const std::string& text, int fontSize = 20, Color bgColor = GRAY, 
               Color hoverColor = DARKGRAY, Color textColor = BLACK)
        : text(text), fontSize(fontSize), bgColor(bgColor), hoverColor(hoverColor), textColor(textColor) {
        updateSize();
    }
    
    void updateSize() {
        width = MeasureText(text.c_str(), fontSize) + 20; // padding
        height = fontSize + 10; // padding
    }
    
    void setText(const std::string& newText) {
        text = newText;
        updateSize();
    }
    
    void setCallback(std::function<void()> cb) { callback = cb; }
    
    bool isInteractable() const override { return true; }
    
    bool isMouseOver() const override {
        Vector2 mousePos = GetMousePosition();
        
        // Scale mouse position to game coordinates (320x240)
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float scale = fminf((float)screenWidth / 320.0f, (float)screenHeight / 240.0f);
        int renderWidth = (int)(320 * scale);
        int renderHeight = (int)(240 * scale);
        int offsetX = (screenWidth - renderWidth) / 2;
        int offsetY = (screenHeight - renderHeight) / 2;
        
        float gameX = (mousePos.x - offsetX) * (320.0f / renderWidth);
        float gameY = (mousePos.y - offsetY) * (240.0f / renderHeight);
        
        return gameX >= x && gameX <= x + width && gameY >= y && gameY <= y + height;
    }
    
    void onClick() override {
        if (callback) callback();
    }
    
    void draw() override {
        if (!visible) return;
        
        hovered = isMouseOver();
        Color currentBg = hovered ? hoverColor : bgColor;
        
        DrawRectangle(x, y, width, height, currentBg);
        
        int textX = x + (width - MeasureText(text.c_str(), fontSize)) / 2;
        int textY = y + (height - fontSize) / 2;
        DrawText(text.c_str(), textX, textY, fontSize, textColor);
    }
};

// Layout container that can arrange elements automatically
class FlexContainer {
public:
    enum LayoutType { VERTICAL, HORIZONTAL, WRAP };
    
private:
    std::vector<std::unique_ptr<UIElement>> elements;
    LayoutType layout;
    int spacing;
    int padding;
    int margin;
    int containerX, containerY;
    int maxWidth, maxHeight;
    bool debugOverflow;
    
    // Layout caching
    bool layoutDirty;
    int lastScreenWidth, lastScreenHeight;
    int lastContainerX, lastContainerY, lastMaxWidth, lastMaxHeight;
    
public:
    FlexContainer(LayoutType layout = VERTICAL, int spacing = 10, int padding = 5, int margin = 2)
        : layout(layout), spacing(spacing), padding(padding), margin(margin),
          containerX(0), containerY(0), maxWidth(320), maxHeight(240), debugOverflow(true),
          layoutDirty(true), lastScreenWidth(0), lastScreenHeight(0),
          lastContainerX(-1), lastContainerY(-1), lastMaxWidth(-1), lastMaxHeight(-1) {}
    
    void setContainer(int x, int y, int maxW = 320, int maxH = 240) {
        if (containerX != x || containerY != y || maxWidth != maxW || maxHeight != maxH) {
            containerX = x;
            containerY = y;
            maxWidth = maxW;
            maxHeight = maxH;
            markLayoutDirty();
        }
    }
    
    void setSpacing(int s) { if (spacing != s) { spacing = s; markLayoutDirty(); } }
    void setPadding(int p) { if (padding != p) { padding = p; markLayoutDirty(); } }
    void setMargin(int m) { if (margin != m) { margin = m; markLayoutDirty(); } }
    void setLayout(LayoutType l) { if (layout != l) { layout = l; markLayoutDirty(); } }
    void setDebugOverflow(bool debug) { debugOverflow = debug; }
    
    void markLayoutDirty() { layoutDirty = true; }
    
    void forceRebuild() {
        layoutDirty = true;
        layoutElements();
    }
    
    void centerHorizontally() {
        if (elements.empty()) return;
        
        // Only recenter if layout changed
        if (!needsLayout()) return;
        
        // Calculate total width of all elements
        int totalWidth = 0;
        for (auto& element : elements) {
            if (element->visible) {
                totalWidth += element->width;
                if (totalWidth > element->width) totalWidth += spacing; // Add spacing between elements
            }
        }
        totalWidth -= spacing; // Remove last spacing
        
        // Center the elements
        int startX = containerX + (maxWidth - totalWidth) / 2;
        int currentX = startX;
        int currentY = containerY + padding;
        
        for (auto& element : elements) {
            if (!element->visible) continue;
            element->setPosition(currentX, currentY);
            currentX += element->width + spacing;
        }
    }
    
    template<typename T, typename... Args>
    T* add(Args&&... args) {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = element.get();
        elements.push_back(std::move(element));
        markLayoutDirty();  // New element added, need to recalculate
        return ptr;
    }
    
    bool needsLayout() {
        if (layoutDirty) return true;
        
        // Check if screen size changed
        int currentScreenWidth = GetScreenWidth();
        int currentScreenHeight = GetScreenHeight();
        if (currentScreenWidth != lastScreenWidth || currentScreenHeight != lastScreenHeight) {
            return true;
        }
        
        // Check if container parameters changed
        if (containerX != lastContainerX || containerY != lastContainerY || 
            maxWidth != lastMaxWidth || maxHeight != lastMaxHeight) {
            return true;
        }
        
        return false;
    }
    
    void layoutElements() {
        if (elements.empty()) return;
        
        // Only recalculate if layout is dirty or screen/container changed
        if (!needsLayout()) return;
        
        int currentX = containerX + padding;
        int currentY = containerY + padding;
        int rowHeight = 0;
        int maxUsedX = currentX;
        int maxUsedY = currentY;
        
        if (debugOverflow) {
            std::cout << "=== Layout RECALCULATED (not cached) ===" << std::endl;
            std::cout << "Container: (" << containerX << ", " << containerY << ") size: " << maxWidth << "x" << maxHeight << std::endl;
            std::cout << "Padding: " << padding << ", Margin: " << margin << ", Spacing: " << spacing << std::endl;
        }
        
        for (size_t i = 0; i < elements.size(); i++) {
            auto& element = elements[i];
            if (!element->visible) continue;
            
            switch (layout) {
                case VERTICAL:
                    element->setPosition(currentX, currentY);
                    currentY += element->height + spacing;
                    maxUsedX = std::max(maxUsedX, currentX + element->width);
                    maxUsedY = std::max(maxUsedY, currentY);
                    break;
                    
                case HORIZONTAL:
                    element->setPosition(currentX, currentY);
                    currentX += element->width + spacing + margin;
                    maxUsedX = std::max(maxUsedX, currentX);
                    maxUsedY = std::max(maxUsedY, currentY + element->height);
                    break;
                    
                case WRAP:
                    // Check if element fits in current row (with margin)
                    int elementEndX = currentX + element->width + margin;
                    int containerEndX = containerX + maxWidth - padding;
                    
                    if (debugOverflow && i == 0) {
                        std::cout << "First element: width=" << element->width << ", elementEndX=" << elementEndX << ", containerEndX=" << containerEndX << std::endl;
                    }
                    
                    if (elementEndX > containerEndX && currentX > containerX + padding) {
                        // Move to next row
                        currentX = containerX + padding;
                        currentY += rowHeight + spacing + margin;
                        rowHeight = 0;
                        
                        if (debugOverflow) {
                            std::cout << "Wrapping element " << i << " to new row at Y=" << currentY << std::endl;
                        }
                    }
                    
                    element->setPosition(currentX, currentY);
                    currentX += element->width + spacing + margin;
                    rowHeight = std::max(rowHeight, element->height);
                    maxUsedX = std::max(maxUsedX, currentX);
                    maxUsedY = std::max(maxUsedY, currentY + element->height);
                    
                    if (debugOverflow) {
                        std::cout << "Element " << i << ": pos=(" << element->x << "," << element->y << ") size=" << element->width << "x" << element->height << std::endl;
                    }
                    break;
            }
        }
        
        // Check for overflow
        if (debugOverflow) {
            bool overflowX = maxUsedX > containerX + maxWidth;
            bool overflowY = maxUsedY > containerY + maxHeight;
            
            if (overflowX || overflowY) {
                std::cout << "*** OVERFLOW DETECTED ***" << std::endl;
                std::cout << "Used area: " << (maxUsedX - containerX) << "x" << (maxUsedY - containerY) << ", Container: " << maxWidth << "x" << maxHeight << std::endl;
                if (overflowX) std::cout << "X overflow: " << (maxUsedX - (containerX + maxWidth)) << " pixels" << std::endl;
                if (overflowY) std::cout << "Y overflow: " << (maxUsedY - (containerY + maxHeight)) << " pixels" << std::endl;
            } else {
                std::cout << "Layout fits within container bounds" << std::endl;
            }
            std::cout << "===================" << std::endl;
        }
        
        // Update cache after successful layout
        layoutDirty = false;
        lastScreenWidth = GetScreenWidth();
        lastScreenHeight = GetScreenHeight();
        lastContainerX = containerX;
        lastContainerY = containerY;
        lastMaxWidth = maxWidth;
        lastMaxHeight = maxHeight;
    }
    
    void draw() {
        for (auto& element : elements) {
            element->draw();
        }
    }
    
    void handleClick() {
        for (auto& element : elements) {
            if (element->isInteractable() && element->isMouseOver()) {
                element->onClick();
                break;
            }
        }
    }
    
    UIElement* getElement(size_t index) {
        if (index < elements.size()) {
            return elements[index].get();
        }
        return nullptr;
    }
    
    size_t size() const { return elements.size(); }
};
