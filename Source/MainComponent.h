#pragma once

#include <JuceHeader.h>
#include "Globals.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void flushSaveData()
    {
        saveData.setProperty(XmlID::SongName, songNameEditor.getText(), nullptr);
        saveData.setProperty(XmlID::MeasureCount, measureCountEditor.getText().getIntValue(), nullptr);
        saveData.setProperty(XmlID::StartMeasure, startMeasureDropdown.getSelectedId(), nullptr);
        saveData.setProperty(XmlID::EndMeasure, endMeasureDropdown.getSelectedId(), nullptr);
        saveData.setProperty(XmlID::SectionMeasureCount, sectionMeasureCountDropdown.getSelectedId(), nullptr);
        saveData.setProperty(XmlID::CurrentSection, currentSectionIndex, nullptr);
        saveData.setProperty(XmlID::SectionMultiplier, sectionMultiplierDropdown.getSelectedId(), nullptr);

        juce::File saveFile(getSavePath() + "\\SaveData.xml");
        saveFile.create();
        saveFile.replaceWithText(saveData.toXmlString(getXmlNoWrapFormat()));
    }
    void loadSaveData()
    {
        juce::File saveFile(getSavePath() + "\\SaveData.xml");
        if (!saveFile.exists())
        {
            saveData = juce::ValueTree("SaveData");
            flushSaveData();
            return;
        }

        saveData = juce::ValueTree::fromXml(saveFile.loadFileAsString());
        if (!saveData.isValid())
            return;

        applyLoadedData(saveData.getProperty(XmlID::SongName), songNameEditor);

        if (applyLoadedData(saveData.getProperty(XmlID::MeasureCount), measureCountEditor))
            updateMeasureDropdowns(false);

        applyLoadedData(saveData.getProperty(XmlID::StartMeasure), startMeasureDropdown);
        applyLoadedData(saveData.getProperty(XmlID::EndMeasure), endMeasureDropdown);
        applyLoadedData(saveData.getProperty(XmlID::SectionMeasureCount), sectionMeasureCountDropdown);
        applyLoadedData(saveData.getProperty(XmlID::CurrentSection), currentSectionIndex);
        applyLoadedData(saveData.getProperty(XmlID::SectionMultiplier), sectionMultiplierDropdown);
    }

    juce::ValueTree saveData;

    struct XmlID
    {
        inline static const char* SongName = "SongName";
        inline static const char* MeasureCount = "MeasureCount";
        inline static const char* StartMeasure = "StartMeasure";
        inline static const char* EndMeasure = "EndMeasure";
        inline static const char* SectionMeasureCount = "SectionMeasureCount";
        inline static const char* CurrentSection = "CurrentSection";
        inline static const char* SectionMultiplier = "SectionMultiplier";

    };

    void updateMeasureDropdown(juce::ComboBox& dropdown, juce::StringArray& itemList)
    {
        int previousValue = dropdown.getSelectedId();
        if (previousValue == 0)
            previousValue = 1;
        if (previousValue > itemList.size())
            previousValue = itemList.size();

        dropdown.clear();
        dropdown.addItemList(itemList, 1);
        dropdown.setSelectedId(previousValue, juce::dontSendNotification);
    }
    void updateMeasureDropdowns(bool notifySectionView = true)
    {
        int measureCount = measureCountEditor.getText().getIntValue();
        juce::StringArray itemList;
        itemList.ensureStorageAllocated(measureCount);
        for (int i = 1; i <= measureCount; i++)
        {
            itemList.add(toString(i));
        }

        updateMeasureDropdown(startMeasureDropdown, itemList);
        updateMeasureDropdown(endMeasureDropdown, itemList);
        updateMeasureDropdown(sectionMeasureCountDropdown, itemList);
        updateMeasureDropdown(sectionMultiplierDropdown, itemList);

        if (notifySectionView)
            updateSection();
    }

    void updateSection() { changeSection(0); }
    void changeSection(int increment)
    {
        if (measureCountEditor.getText().isEmpty())
            return;

        int previousSectionIndex = currentSectionIndex;
        currentSectionIndex += increment;

        int startMeasure = startMeasureDropdown.getSelectedId();
        int endMeasure = endMeasureDropdown.getSelectedId();
        int sectionLength = sectionMeasureCountDropdown.getSelectedId();
        if (startMeasure == 0 || endMeasure == 0 || sectionLength == 0)
            return;

        int sectionStart = startMeasure + currentSectionIndex;
        int sectionEnd = sectionStart + (sectionLength - 1);

        if (currentSectionIndex < 0)
        {
            sectionLength--;
            if (sectionLength <= 0)
            {
                currentSectionIndex = previousSectionIndex;
                return;
            }

            sectionMeasureCountDropdown.setSelectedId(sectionLength, juce::dontSendNotification);
            sectionEnd = endMeasure;
            sectionStart = endMeasure - (sectionLength - 1);
            currentSectionIndex = sectionStart - startMeasure;
        }
        else if (sectionEnd > endMeasure)
        {
            sectionLength++;
            if (isSectionFinished(startMeasure, endMeasure, sectionLength))
            {
                currentSectionIndex = previousSectionIndex;
                return;
            }

            sectionMeasureCountDropdown.setSelectedId(sectionLength, juce::dontSendNotification);
            currentSectionIndex = 0;
            sectionStart = startMeasure + currentSectionIndex;
            sectionEnd = sectionStart + (sectionLength - 1);
        }

        currentSectionView.setText(toString(sectionStart) + "-" + toString(sectionEnd));
        flushSaveData();
    }

    bool isSectionFinished(int startMeasure, int endMeasure, int sectionLength) { return endMeasure - startMeasure + 1 < sectionLength; }

    int currentSectionIndex = 0;

    //==============================================================================
    //UI

    struct TitleManager
    {
        TitleManager() {}
        ~TitleManager()
        {
            for (auto& c : m_titles)
                delete c;
        }

        //returns the new title so that you can add it to a parent and make visible
        juce::TextButton* addTitle(juce::Component* component, juce::String title) { return addTitle(*component, title); }
        //returns the new title so that you can add it to a parent and make visible
        juce::TextButton* addTitle(juce::Component& component, juce::String title)
        {
            jassert(!m_components.contains(&component)); //component shouldn't already have a title

            auto newTitle = new juce::TextButton(title);
            m_titles.add(newTitle);
            m_components.add(&component);

            return newTitle;
        }

        juce::TextButton* getTitle(juce::Component* component) { return getTitle(*component); }
        juce::TextButton* getTitle(juce::Component& component)
        {
            for (size_t i = 0; i < m_titles.size(); i++)
            {
                if (m_components[i] == &component)
                    return m_titles[i];
            }
            jassertfalse; //could not find title paired with this component
            return nullptr;
        }

        void setTitleBounds(juce::Component* component, Bounds bounds) { getTitle(component)->setBounds(bounds); }
        void setTitleBounds(juce::Component& component, Bounds bounds) { getTitle(component)->setBounds(bounds); }

    private:
        vArray<juce::TextButton*> m_titles;
        vArray<juce::Component*> m_components;
    };

    void paint (juce::Graphics&) override;
    void resized() override;

    TitleManager titleManager;

    vArray<juce::TextButton*> titles;

    juce::TextEditor songNameEditor;
    juce::TextEditor measureCountEditor;
    juce::ComboBox startMeasureDropdown;
    juce::ComboBox endMeasureDropdown;
    juce::ComboBox sectionMeasureCountDropdown;

    juce::TextButton previousSection{ "<" };
    juce::TextButton nextSection{ ">" };

    juce::ComboBox sectionMultiplierDropdown;
    juce::TextButton previousSectionMultiplier{ "<" };
    juce::TextButton nextSectionMultiplier{ ">" };

    juce::TextEditor currentSectionView;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
