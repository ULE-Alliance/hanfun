# Hanfun Code Generators

This gem provides a command line tool to generate new interfaces, services and profiles for the [HAN-FUN](https://github.com/ULE-Alliance/hanfun) project.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'hanfun'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install hanfun

## Usage

In order to generate a new interface issue the following command.

### Interfaces

The general command to generate the code for a new interface is the following:

    $ hanfun interface <name> <uid> [short] \
        [-a attribute_1:type:uid:acl:spec ... attribute_n:type:uid:acl:spec] \
        [-c command_1:uid:type:server_spec:client_spec ... command_n:uid:type:server_spec:client_spec]

If for example we have the following interface:

* General

| Field | Value  | Type     | Comment                                                                                          |
| :-:   | :-:    | :-:      | ---                                                                                              |
| name  | Logger | Required | The name of the interface in underscore notation, e.g., SimpleTemperature -> simple_temperature. |
| uid   | 0x1234 | Required | The UID for the interface.                                                                       |
| short | LOG    | Optional | The string used to namespace optional attibutes                                                  |

* Attributes

| Name    | Type     | UID | ACL | Spec | Comment                                                                                                            |
| :-:     | :-:      | :-: | :-: | :-:  | ---                                                                                                                |
| enable  | uint8_t  | 1   | RW  | M    | This will generate a mandatory read-write attribute that is backed by a uint8_t variable, named state with UID = 1 |
| nevents | uint16_t | 2   | R   | O    | This will generate an optional read only attribute that is backed by a uint16_t variable, named level with UID = 2 |

* Commands

| Name    | UID | Type | Server Spec | Client Spec | Comment                                                                                                                                                                                                       |
| :-:     | :-: | :-:  | :-:         | :-:         | ---                                                                                                                                                                                                           |
| error   | 1   | e    | M           | M           | This will generate a command that will be sent from the Server role to the Client Role of the interface, named failed, with an UID = 1, that is mandatory for both Server and Clients to implement.           |
| warning | 2   | e    | M           | O           | This will generate a command that will be sent from the Server role to the Client Role of the interface, named warning, with an UID = 2, that is mandatory for Servers to implement but optional for Clients. |
| info    | 3   | e    | O           | O           | This will generate a command that will be sent from the Server role to the Client Role of the interface, named info, with an UID = 3, that is optional for both Servers and Clients to implement.             |
| clear   | 4   | c    | O           | M           | This will generate a command that will be sent from the Client role to the Server Role of the interface, named clear, with an UID = 4, that is optional for Servers and mandatory for Clients to implement.   |

The command whould be:

    $ hanfun interface logger 0x1234 LOG \
        -a enable:uint8_t:1:rw:m nevents:uint16_t:2:r:o \
        -c error:1:e:m:m warning:2:e:m:o info:3:e:o:o clear:4:c:o:o

### Core Services & Interfaces

For genenrate a new service the general command is:

    $ hanfun core <name> <uid> [short] \
        [-a attribute_1:type:uid:acl:spec ... attribute_n:type:uid:acl:spec] \
        [-c command_1:uid:type:server_spec:client_spec ... command_n:uid:type:server_spec:client_spec]

Where the fields are have the same meaning as above. To generate a core interface add a '-i' flag to the above command, e.g.:

    $ hanfun core -i ...

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release` to create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

### TODO

 * Add profile generator.

## Contributing

1. Fork it ( https://github.com/ULE-Alliance/hanfun )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request
