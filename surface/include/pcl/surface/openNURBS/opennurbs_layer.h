/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2011 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

#if !defined(OPENNURBS_LAYER_INC_)
#define OPENNURBS_LAYER_INC_

class ON_CLASS ON_Layer : public ON_Object {
    ON_OBJECT_DECLARE(ON_Layer);

  public:
    ON_Layer();
    ~ON_Layer();
    // C++ default copy construction and operator= work fine.
    // Do not add custom versions.

    //////////////////////////////////////////////////////////////////////
    //
    // ON_Object overrides

    /*
    Description:
      Tests an object to see if its data members are correctly
      initialized.
    Parameters:
      text_log - [in] if the object is not valid and text_log
          is not NULL, then a brief englis description of the
          reason the object is not valid is appened to the log.
          The information appended to text_log is suitable for
          low-level debugging purposes by programmers and is
          not intended to be useful as a high level user
          interface tool.
    Returns:
      @untitled table
      true     object is valid
      false    object is invalid, uninitialized, etc.
    Remarks:
      Overrides virtual ON_Object::IsValid
    */
    ON_BOOL32 IsValid(ON_TextLog *text_log = NULL) const;

    void Dump(ON_TextLog &) const; // for debugging

    ON_BOOL32 Write(ON_BinaryArchive & // serialize definition to binary archive
    ) const;

    ON_BOOL32 Read(ON_BinaryArchive & // restore definition from binary archive
    );

    ON::object_type ObjectType() const;

    ON_UUID ModelObjectId() const;

    //////////////////////////////////////////////////////////////////////
    //
    // Interface

    void Default();

    bool SetLayerName(const char *);
    bool SetLayerName(const wchar_t *);
    const ON_wString &LayerName() const;

    /*
    Description:
      The string returned by ON_Layer::LayerNameReferenceDelimiter()
      is used to separate the name of a reference file from the name of
      the layer in the file.
    Example:
      If a layer named "electrical" is in a file named "house plan.3dm"
      and "house plan.3dm" is a reference file in a Rhino worksession,
      then Rhino's user interface will use the string
      "house plan : electrical" to identify the layer.
    Returns:
      " : " (null terminated string space,colon,space)
    Remarks:
      Rhino does not save the names of reference files in 3dm archives.
      Reference file names are used as runtime decorations.
    */
    static const wchar_t *LayerNameReferenceDelimiter();

    /*
    Description:
      The string "::" (colon,colon) returned by LayerNamePathDelimiter()
      is used to separate parent and child layer names.
    Example:
      If a model of a building has "level 1" and "level 2" as top level
      layers, an architect might choose to have a "fixtures" sublayer
      on each level.  The complete layer names would be
      "level 1::fixtures" and "level 2::fixtures".
    Returns:
      "::" (null terminated string colon,colon)
    */
    static const wchar_t *LayerNamePathDelimiter();

    /*
    Description:
      Get a layer name's "leaf" level name.
    Example:
      If a layer name is "refernce file : alpha::beta::gamma",
      then ON_Layer::GetLeafName() returns "gamma"
    Returns:
      True if the layer has a valid non-empty leaf name.
    */
    static bool GetLeafName(const wchar_t *layer_name, ON_wString &leaf_name);

    /*
    Description:
      Get the layer's "parent" path name.
    Example:
      If a layer name is "refenence file : alpha::beta::gamma", then
      ON_Layer::GetParentPathName() returns "alpha::beta"
    Returns:
      True if the layer has a valid non-empty parent path name.
    */
    static bool GetParentName(const wchar_t *layer_name,
                              ON_wString &parent_path_name);

    /*
    Description:
      Remove any "reference : " prefix from a layer's name.
    Parameters:
      layer_name - [in]
      layer_path_name - [out]
        layer_name with any reference prefix removed.
    Example:
      If a layer name is "refenence file : alpha::beta::gamma", then
      ON_Layer::RemoveReferenceName() returns "alpha::beta::gamma"
    Returns:
      True if layer_path_name is non-empty. If no reference prefix was present,
      then the returned layer_path_name is identical to the input layer_name.
    */
    static bool RemoveReferenceName(const wchar_t *layer_name,
                                    ON_wString &layer_path_name);

    /*
    Description:
      Get the layer's reference name.
    Example:
      If a layer name is "refenence file : alpha::beta::gamma", then
      ON_Layer::GetReferenceFileName() returns "refenence file"
    Returns:
      True if the layer has a valid non-empty reference file name.
    */
    static bool GetReferenceName(const wchar_t *layer_name,
                                 ON_wString &reference_name);

    /*
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then checks for setting for
        that specific viewport.
        If viewport_id is nil, then checks for any viewport settings.
    Returns:
      True if the layer has per viewport settings.
    */
    bool HasPerViewportSettings(const ON_UUID &viewport_id) const;

    /*
    Description:
      Copies all per viewport settings
    Parameters:
      source - [in]
        viewport id to copy all per viewport settings from
      destination - [in]
        viewport od to copy all per viewport settings to
    Returns:
      True if the settings could be copied, False if no per-viewport
      settings exist for the source viewport id
    */
    bool CopyPerViewportSettings(ON_UUID source, ON_UUID destination);

    /*
    Description:
      Delete per viewport layer settings.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the settings for that
        viewport are deleted.  If viewport_id is nil, then all
        per viewport settings are deleted.
    */
    void DeletePerViewportSettings(const ON_UUID &viewport_id) const;

    /*
    Description:
      Cull unused per viewport layer settings.
    Parameters:
      viewport_id_count - [in]
      viewport_id_list - [in]
        Settings for any viewports NOT in the viewport_id_list[]
        are culled.
    */
    void CullPerViewportSettings(int viewport_id_count,
                                 const ON_UUID *viewport_id_list);

    /*
    Description:
      The PerViewportSettingsCRC() can be used to determine
      when layers have different per viewport settings.
    */
    ON__UINT32 PerViewportSettingsCRC() const;

    /*
    Description:
      Set the color used by objects on this layer that do
      not have a per object color set
    Parameters:
      layer_color - [in]
        Passing ON_UNSET_COLOR will clear the settings.
      viewport_id - [in]
        If viewport_id is not nil, then the setting applies only
        to the viewport with the specified id.
    */
    void SetColor(ON_Color layer_color); // layer display color
    void SetColor(ON_Color layer_color, const ON_UUID &viewport_id);

    /*
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting to use
        for a specific viewport is returned.
    Returns:
      The color used by objects on this layer that do
      not have a per object color set.
    */
    ON_Color Color() const;
    ON_Color Color(const ON_UUID &viewport_id) const;

    /*
    Description:
      Remove any per viewport layer color setting so the
      layer's overall setting will be used for all viewports.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting for this
        viewport will be deleted.  If viewport_id is nil,
        the all per viewport layer color settings will be removed.
    */
    void DeletePerViewportColor(const ON_UUID &viewport_id);

    /*
    Description:
      Set the plotting color used by objects on this layer that do
      not have a per object plotting color set
    Parameters:
      plot_color - [in]
        Passing ON_UNSET_COLOR will clear the settings.
      viewport_id - [in]
        If viewport_id is not nil, then the setting applies only
        to the viewport with the specified id.
    */
    void SetPlotColor(ON_Color plot_color); // plotting color
    void SetPlotColor(ON_Color plot_color,
                      const ON_UUID &viewport_id); // plotting color

    /*
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting to use
        for a specific viewport is returned.
    Returns:
      The plotting color used by objects on this layer that do
      not have a per object color set.
    */
    ON_Color PlotColor() const;
    ON_Color PlotColor(const ON_UUID &viewport_id) const;

    /*
    Description:
      Remove any per viewport plot color setting so the
      layer's overall setting will be used for all viewports.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting for this
        viewport will be deleted.  If viewport_id is nil,
        the all per viewport plot color settings will be removed.
    */
    void DeletePerViewportPlotColor(const ON_UUID &viewport_id);

    /*
    Description:
      Set the index of the linetype used by objects on this layer that do
      not have a per object lintypes
    Parameters:
      linetype_index - [in]
        Passing -1 will clear the setting.
    */
    bool SetLinetypeIndex(int linetype_index);

    /*
    Returns:
      The index of the linetype used by objects on this layer that do
      not have a per object linetype set.
    */
    int LinetypeIndex() const;

    /*
    Returns:
      Returns true if objects on layer are visible.
    Remarks:
      Does not inspect per viewport settings.
    See Also:
      ON_Layer::SetVisible
    */
    bool IsVisible() const;

    /*
    Returns:
      Returns true if objects on layer are visible.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the visibility setting
        for that viewport is returned.  If viewport_id
        is nil, then true is returned if the layer is visible
        in some viewport.
    */
    bool IsVisible(const ON_UUID &viewport_id) const;

    /*
    Description:
      Controls layer visibility
    Parameters:
      bVisible - [in] true to make layer visible,
                      false to make layer invisible
      viewport_id - [in]
        If viewport_id is not nil, then the setting applies only
        to the viewport with the specified id.
    See Also:
      ON_Layer::IsVisible
    */
    void SetVisible(bool bVisible);
    void SetVisible(bool bVisible, const ON_UUID &viewport_id);

    /*
    Description:
      Remove any per viewport visibility setting so the
      layer's overall setting will be used for all viewports.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting for this
        viewport will be deleted.  If viewport_id is nil,
        the all per viewport visibility settings will be removed.
    */
    void DeletePerViewportVisible(const ON_UUID &viewport_id);

    /*
    Returns:
      Returns true if objects on layer are locked.
    See Also:
      ON_Layer::SetLocked
    */
    bool IsLocked() const;

    /*
    Description:
      Controls layer locked
    Parameters:
      bLocked - [in] True to lock layer
                     False to unlock layer
    See Also:
      ON_Layer::IsLocked
    */
    void SetLocked(bool bLocked);

    /*
    Returns:
      Value of (IsVisible() && !IsLocked()).
    */
    bool IsVisibleAndNotLocked() const;

    /*
    Returns:
      Value of (IsVisible() && IsLocked()).
    */
    bool IsVisibleAndLocked() const;

    //////////
    // Index of render material for objects on this layer that have
    // MaterialSource() == ON::material_from_layer.
    // A material index of -1 indicates no material has been assigned
    // and the material created by the default ON_Material constructor
    // should be used.
    bool SetRenderMaterialIndex(int); // index of layer's rendering material
    int RenderMaterialIndex() const;

    bool SetLayerIndex(int); // index of this layer;
    int LayerIndex() const;

    bool SetIgesLevel(int); // IGES level for this layer
    int IgesLevel() const;

    /*
    Description:
      Get the weight (thickness) of the plotting pen.
    Returns:
      Thickness of the plotting pen in millimeters.
      A thickness of  0.0 indicates the "default" pen weight should be used.
      A thickness of -1.0 indicates the layer should not be printed.
    */
    double PlotWeight() const;
    double PlotWeight(const ON_UUID &viewport_id) const;

    /*
    Description:
      Get the weight of the plotting pen.
    Parameters:
      plot_weight_mm - [in] Set the thickness of the plotting pen in
    millimeters. 0.0 means use the default pen width which is a Rhino app
    setting. -1.0 means layer does not print (still displays on the screen)
    */
    void SetPlotWeight(double plot_weight_mm);
    void SetPlotWeight(double plot_weight_mm, const ON_UUID &viewport_id);

    /*
    Description:
      Remove any per viewport plot weight setting so the
      layer's overall setting will be used for all viewports.
    Parameters:
      viewport_id - [in]
        If viewport_id is not nil, then the setting for this
        viewport will be deleted.  If viewport_id is nil,
        the all per viewport plot weight settings will be removed.
    */
    void DeletePerViewportPlotWeight(const ON_UUID &viewport_id);

    /*
    Description:
      Use UpdateViewportIds() to change viewport ids in situations
      like merging when a viewport id conflict requires the viewport
      ids in a file to be changed.
    Returns:
      Number of viewport ids that were updated.
    */
    int UpdateViewportIds(const ON_UuidPairList &viewport_id_map);

  public:
    int m_layer_index; // index of this layer
    ON_UUID m_layer_id;
    ON_UUID m_parent_layer_id; // Layers are origanized in a hierarchical
                               // structure (like file folders).
                               // If a layer is in a parent layer,
                               // then m_parent_layer_id is the id of
                               // the parent layer.

    int m_iges_level; // IGES level number if this layer was made during IGES
                      // import

    // Rendering material:
    //   If you want something simple and fast, set
    //   m_material_index to the index of your rendering material
    //   and ignore m_rendering_attributes.
    //   If you are developing a fancy plug-in renderer, and a user is
    //   assigning one of your fabulous rendering materials to this
    //   layer, then add rendering material information to the
    //   m_rendering_attributes.m_materials[] array.
    //
    // Developers:
    //   As soon as m_rendering_attributes.m_materials[] is not empty,
    //   rendering material queries slow down.  Do not populate
    //   m_rendering_attributes.m_materials[] when setting
    //   m_material_index will take care of your needs.
    int m_material_index;
    ON_RenderingAttributes m_rendering_attributes;

    int m_linetype_index; // index of linetype

    // Layer display attributes.
    //   If m_display_material_id is nil, then m_color is the layer color
    //   and defaults are used for all other display attributes.
    //   If m_display_material_id is not nil, then some complicated
    //   scheme is used to decide what objects on this layer look like.
    //   In all cases, m_color is a good choice if you don't want to
    //   deal with m_display_material_id.  In Rhino, m_display_material_id
    //   is used to identify a registry entry that contains user specific
    //   display preferences.
    ON_Color m_color;
    ON_UUID m_display_material_id;

    // Layer printing (plotting) attributes.
    ON_Color m_plot_color;   // printing color
                             // ON_UNSET_COLOR means use layer color
    double m_plot_weight_mm; // printing pen thickness in mm
                             //  0.0 means use the default width (a Rhino app
                             //  setting)
                             // -1.0 means layer does not print (still visible
                             // on screen)
    ON_wString m_name;

    bool m_bVisible;  // If true, objects on this layer are visible.
    bool m_bLocked;   // If true, objects on this layer cannot be modified.
    bool m_bExpanded; // If true, when the layer table is displayed in
                      // a tree control then the list of child layers is
                      // shown in the control.

    //////////////////////////////////////////////////////////////
    //
    // Tools for saving layer settings.
    //
    enum LAYER_SETTINGS {
        no_layer_settings = 0,
        userdata_settings = 1,
        color_settings = 2,
        plot_color_settings = 4,
        plot_weight_settings = 8,
        visible_settings = 16,
        locked_settings = 32,
        all_layer_settings = 0xFFFFFFFF
    };

    /*
    Returns:
      Bits in the returned value indicate if there are differences
      between layer0 and layer1.  For example, if the layers have
      difference color, then the returned value would have the
      "color" bit set.
    */
    static unsigned int Differences(const ON_Layer &layer0,
                                    const ON_Layer &layer1);

    /*
    Description:
      Use settings_values and settings to set the specified values
      on this layer.
    Parameters:
      settings_values - [in]
      settings - [in]
        LAYER_SETTINGS bits specify which values of this
        should be set from settings_values.
    */
    void Set(unsigned int settings, const ON_Layer &settings_values);

    /*
    Description:
      Saves current values of the specified settings so
      they can be retrieved by GetSettings().
    Parameters:
      settings - [in]
        LAYER_SETTINGS bits specify which values to save.
        if 0 == settings, then all saved settings are deleted.
      bUpdate - [in]
        If true, then previously saved settings for properties
        not identified by the settings paramter are left intact.
        If false, all previously saved settings are removed.
    */
    void SaveSettings(unsigned int settings, bool bUpdate);

    /*
    Returns:
      0 if the layer does not have saved settings.
      Nonzero value with LAYER_SETTINGS bits specifying which settings
      are saved.  The saved that can be retrieved by calling
      GetSavedSettings().
    */
    unsigned int SavedSettings() const;

    /*
    Description:
      Gets values of the saved settings.
    Parameters:
      layer - [in/out]
        values of saved settings are set and all other values are
        left unchanged.
      settings - [out]
        LAYER_SETTINGS bits specify which layer values were set
        by this call.
    Returns:
      True if there were saved settings.
    */
    bool GetSavedSettings(ON_Layer &layer, unsigned int &settings) const;

  private:
    // The m__runtime_flags are used to speed queries that require
    // checking user data.  This field is not saved in persistent
    // archives and its interpretation is subject to change.
    unsigned char m__runtime_flags;
};

#endif
