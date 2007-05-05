"""
    Simple mode

    Use SUMMON in simple single-window mode.
    
    In single-window mode, these global functions can be used to interact 
    directly with the summon window.  This is useful for people not familiar
    with object-oriented programming.
    
    Scripts executed by bin/summon have simple single-window mode loaded by
    default.
"""

import inspect

import summon


_w = summon.get_summon_window

#=============================================================================
# these are wrappers to support the simple global function interface
#



def _make_func(get_obj, func):
    """wrap a method call into a function call"""
    
    regargs, varargs, varkwargs, defaults = inspect.getargspec(func)
    argnames = list(regargs)
    if varargs:
        argnames.append(varargs)
    if varkwargs:
        argnames.append(varkwargs)
    signature = inspect.formatargspec(regargs[1:], varargs, varkwargs, defaults,
                                      formatvalue=lambda value: "")[1:-1]
    new_func = eval("lambda %s: __call_summon_func__(_w(), %s)" % \
                    (signature, signature),
                    {"__call_summon_func__": func,
                     "_w": get_obj})

    new_func.__name__ = func.__name__
    new_func.__doc__ = func.__doc__
    new_func.__dict__.update(func.__dict__) 
    return new_func



_globals = globals()

_name_changes = {"set_name": "set_window_name",
                 "get_name": "get_window_name",
                 "set_size": "set_window_size",
                 "get_size": "get_window_size",
                 "set_position": "set_window_position",
                 "get_position": "get_window_position",
                 "close": "close_window"}

_exclude = set(["activate", "duplicate"])

# window function wrappers
for name, func in summon.Window.__dict__.iteritems():
    if name.startswith("__") or name in _exclude:
        continue
    
    # change some function names
    if name in _name_changes:
        name = _name_changes[name]
    
    _globals[name] = _make_func(_w, func)
    _globals[name].__name__ = name
