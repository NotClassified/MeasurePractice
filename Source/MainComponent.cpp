#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(songNameEditor);
    songNameEditor.setFont(getMonoFont());
    songNameEditor.setTextToShowWhenEmpty("Song Name", juce::Colours::grey);
    songNameEditor.onTextChange = [=]
    {
        flushSaveData();
    };
    
    addAndMakeVisible(titleManager.addTitle(measureCountEditor, "Measure Count"));
    addAndMakeVisible(measureCountEditor);
    measureCountEditor.setFont(getMonoFont());
    measureCountEditor.onTextChange = [=]
    {
        updateMeasureDropdowns();
    };

    addAndMakeVisible(titleManager.addTitle(startMeasureDropdown, "Start Measure"));
    addAndMakeVisible(startMeasureDropdown);
    startMeasureDropdown.setEditableText(true);
    startMeasureDropdown.onChange = [=]
    {
        if (startMeasureDropdown.getSelectedId() == 0)
            startMeasureDropdown.setSelectedId(startMeasureDropdown.getText().getIntValue(), juce::dontSendNotification);
        updateSection();
    };

    addAndMakeVisible(titleManager.addTitle(endMeasureDropdown, "End Measure"));
    addAndMakeVisible(endMeasureDropdown);
    endMeasureDropdown.setEditableText(true);
    endMeasureDropdown.onChange = [=]
    {
        if (endMeasureDropdown.getSelectedId() == 0)
            endMeasureDropdown.setSelectedId(endMeasureDropdown.getText().getIntValue(), juce::dontSendNotification);
        updateSection();
    };

    addAndMakeVisible(titleManager.addTitle(sectionMeasureCountDropdown, "Section Length"));
    addAndMakeVisible(sectionMeasureCountDropdown);
    sectionMeasureCountDropdown.onChange = [=]
    {
        updateSection();
    };

    addAndMakeVisible(previousSection);
    previousSection.onClick = [=]
    {
        changeSection(-1);
    };
    addAndMakeVisible(nextSection);
    nextSection.onClick = [=]
    {
        changeSection(1);
    };


    addAndMakeVisible(sectionMultiplier);
    sectionMultiplier.setEditableText(false);

    addAndMakeVisible(previousSectionMultiplier);
    previousSectionMultiplier.onClick = [=]
    {
        changeSection(-1 * sectionMultiplier.getSelectedId());
    };
    addAndMakeVisible(nextSectionMultiplier);
    nextSectionMultiplier.onClick = [=]
    {
        changeSection(sectionMultiplier.getSelectedId());
    };

    addAndMakeVisible(titleManager.addTitle(currentSectionView, "Current Section"));
    addAndMakeVisible(currentSectionView);
    currentSectionView.setFont(getMonoFont());
    currentSectionView.setReadOnly(true);

    for (auto& c : getChildren())
        c->setTransform(juce::AffineTransform::scale(3.5f));
    setSize (1400, 600);


    loadSaveData();
    updateSection();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    Bounds localBounds = getLocalBounds();

    songNameEditor.setBounds(localBounds.removeFromTop(25).withWidth(400));
    {
        Bounds titleBounds = localBounds.removeFromTop(25);
        Bounds bounds2 = localBounds.removeFromTop(25);

        titleManager.setTitleBounds(measureCountEditor, titleBounds.removeFromLeft(100));
        measureCountEditor.setBounds(bounds2.removeFromLeft(100));

        titleManager.setTitleBounds(startMeasureDropdown, titleBounds.removeFromLeft(100));
        startMeasureDropdown.setBounds(bounds2.removeFromLeft(100));

        titleManager.setTitleBounds(endMeasureDropdown, titleBounds.removeFromLeft(100));
        endMeasureDropdown.setBounds(bounds2.removeFromLeft(100));

        titleManager.setTitleBounds(sectionMeasureCountDropdown, titleBounds.removeFromLeft(100));
        sectionMeasureCountDropdown.setBounds(bounds2.removeFromLeft(100));

    }
    {
        Bounds bounds2 = localBounds.removeFromTop(25);
        previousSection.setBounds(bounds2.removeFromLeft(25));
        nextSection.setBounds(bounds2.removeFromLeft(25));

        bounds2.removeFromLeft(25);

        previousSectionMultiplier.setBounds(bounds2.removeFromLeft(25));
        sectionMultiplier.setBounds(bounds2.removeFromLeft(50));
        nextSectionMultiplier.setBounds(bounds2.removeFromLeft(25));
    }
    currentSectionView.setBounds(localBounds.removeFromTop(25).removeFromLeft(200));
}
