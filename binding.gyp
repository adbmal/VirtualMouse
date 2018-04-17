{
  "targets": [
    {
      "target_name": "screen",
      "sources": [
        "IScreen.cc",
        "screen.cc",
      ],
      'link_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
        ],
      },
      'conditions': [
          ['OS=="mac"', {
            'sources': [
              "OSXScreen.mm",
            ],
          }],
          ['OS=="win"', {
            'defines': [
              'WINDOWS_SPECIFIC_DEFINE',
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