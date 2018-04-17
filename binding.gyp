{
  "targets": [
    {
      "target_name": "screen",
      "sources": [
        "OSXScreen.mm",
        "IScreen.cc",
        "screen.cc",
      ],
      'link_settings': {
        'libraries': [
          '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
        ],
      },
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}