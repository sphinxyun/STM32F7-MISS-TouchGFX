#include <gui/bootscreen_screen/bootScreenView.hpp>

bootScreenView::bootScreenView()
{

}

void bootScreenView::setupScreen()
{
    bootScreenViewBase::setupScreen();
}

void bootScreenView::tearDownScreen()
{
    bootScreenViewBase::tearDownScreen();
}

void bootScreenView::incBrightness() {
	presenter->incBrightness();
}

void bootScreenView::decBrightness() {
	presenter->decBrightness();
}
