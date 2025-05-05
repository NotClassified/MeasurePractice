#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(songNameEditor);
    songNameEditor.setFont(getMonoFont());
    songNameEditor.setTextToShowWhenEmpty("Song Name", juce::Colours::grey);
    songNameEditor.onTextChange = [=]
    {
        saveData.setProperty(XmlID::SongName, songNameEditor.getText(), nullptr);
        flushSaveData();
    };
    
    addAndMakeVisible(titleManager.addTitle(measureCountEditor, "Measure Count"));
    addAndMakeVisible(measureCountEditor);
    measureCountEditor.setFont(getMonoFont(75));
    measureCountEditor.onTextChange = [=]
    {
        updateMeasureDropdowns();
        saveData.setProperty(XmlID::MeasureCount, measureCountEditor.getText().getIntValue(), nullptr);
        flushSaveData();
    };

    addAndMakeVisible(titleManager.addTitle(startMeasureDropdown, "Start Measure"));
    addAndMakeVisible(startMeasureDropdown);
    startMeasureDropdown.onChange = [=]
    {
        updateSection();
        saveData.setProperty(XmlID::StartMeasure, startMeasureDropdown.getSelectedId(), nullptr);
        flushSaveData();
    };

    addAndMakeVisible(titleManager.addTitle(endMeasureDropdown, "End Measure"));
    addAndMakeVisible(endMeasureDropdown);
    endMeasureDropdown.onChange = [=]
    {
        updateSection();
        saveData.setProperty(XmlID::EndMeasure, endMeasureDropdown.getSelectedId(), nullptr);
        flushSaveData();
    };

    addAndMakeVisible(titleManager.addTitle(sectionMeasureCountDropdown, "Section Length"));
    addAndMakeVisible(sectionMeasureCountDropdown);
    sectionMeasureCountDropdown.onChange = [=]
    {
        updateSection();
        saveData.setProperty(XmlID::SectionMeasureCount, sectionMeasureCountDropdown.getSelectedId(), nullptr);
        flushSaveData();
    };

    addAndMakeVisible(previousSection);
    previousSection.onClick = [=]
    {
        changeSection(-1);
        saveData.setProperty(XmlID::CurrentSection, currentSectionIndex, nullptr);
        flushSaveData();
    };

    addAndMakeVisible(nextSection);
    nextSection.onClick = [=]
    {
        changeSection(1);
        saveData.setProperty(XmlID::CurrentSection, currentSectionIndex, nullptr);
        flushSaveData();
    };

    addAndMakeVisible(titleManager.addTitle(currentSectionView, "Current Section"));
    addAndMakeVisible(currentSectionView);
    currentSectionView.setFont(getMonoFont(75));
    currentSectionView.setReadOnly(true);


    loadSaveData();
    applySaveData();

    setSize (600, 400);
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

    songNameEditor.setBounds(localBounds.removeFromTop(25));
    {
        Bounds titleBounds = localBounds.removeFromTop(100);
        Bounds bounds2 = localBounds.removeFromTop(100);

        titleManager.setTitleBounds(measureCountEditor, titleBounds.removeFromLeft(300));
        measureCountEditor.setBounds(bounds2.removeFromLeft(300));

        titleManager.setTitleBounds(startMeasureDropdown, titleBounds.removeFromLeft(300));
        startMeasureDropdown.setBounds(bounds2.removeFromLeft(300));

        titleManager.setTitleBounds(endMeasureDropdown, titleBounds.removeFromLeft(300));
        endMeasureDropdown.setBounds(bounds2.removeFromLeft(300));

        titleManager.setTitleBounds(sectionMeasureCountDropdown, titleBounds.removeFromLeft(300));
        sectionMeasureCountDropdown.setBounds(bounds2.removeFromLeft(300));

    }
    {
        Bounds bounds2 = localBounds.removeFromTop(100);
        previousSection.setBounds(bounds2.removeFromLeft(100));
        nextSection.setBounds(bounds2.removeFromLeft(100));
    }
    currentSectionView.setBounds(localBounds.removeFromTop(100).removeFromLeft(600));
}
