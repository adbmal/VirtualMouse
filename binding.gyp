{
  "targets": [
    {
      "target_name": "screen",
      "sources": [
        "IScreen.cc",
        "screen.cc",
      ],
      'conditions': [
          ['OS=="mac"', {
            'sources': [
              "OSXScreen.mm",
            ],
            'link_settings': {
              'libraries': [
                '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
              ],
            },
          }],
          ['OS=="win"', {
            'sources': [
              "MSScreen.cc",
            ],
          }, { # OS != "win",
            'defines': [
              'NON_WINDOWS_DEFINE',
            ],
          }]
      ],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}